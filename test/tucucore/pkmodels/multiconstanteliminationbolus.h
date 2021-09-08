#ifndef MULTICONSTANTELIMINATIONBOLUS_H
#define MULTICONSTANTELIMINATIONBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

enum class MultiConstantEliminationBolusExponentials : int { P0, P1 };
enum class MultiConstantEliminationBolusCompartments : int { First, Second };


/// \ingroup TucuCore
/// \brief Intake interval calculator for tests. It is a bolus with a linear slope
///
/// 4 parameters:
///     - A
///     - S
///     - R
///     - M
/// The equation is the following:
/// C0(t) = max(0.0 , (D + residual * R0) * ( 1 - t * S0) * M0 + A0)
/// C1(t) = max(0.0 , (D + residual * R1) * ( 1 - t * S1) * M1 + A1)
/// It allows to test other components with a simple equation:
///     - Easy to calculate AUC
///     - Easy to calculate residual
///     - Easy to calculate percentiles depending on variability of A
///
/// When using R=0, S=0 and A=0, the concentration is simply the dose
///

class MultiConstantEliminationBolus : public IntakeIntervalCalculatorBase<2, MultiConstantEliminationBolusExponentials>
{
   INTAKEINTERVALCALCULATOR_UTILS(MultiConstantEliminationBolus)
public:
    /// \brief Constructor
    MultiConstantEliminationBolus() : IntakeIntervalCalculatorBase<2, MultiConstantEliminationBolusExponentials> (new PertinentTimesCalculatorStandard())
    {

    }

    typedef MultiConstantEliminationBolusExponentials Exponentials;


    static std::vector<std::string> getParametersId()
    {
        return {"TestS0", "TestA0", "TestR0", "TestM0, TestS1, TestA1, TestR1, TestM1"};
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {

        if(!checkCondition(_parameters.size() >= 8, "The number of parameters should be equal to 8.")) {
            return false;
        }

        m_D = _intakeEvent.getDose();
        m_S0 = _parameters.getValue(ParameterId::TestS0);
        m_A0 = _parameters.getValue(ParameterId::TestA0);
        m_R0 = _parameters.getValue(ParameterId::TestR0);
        m_M0 = _parameters.getValue(ParameterId::TestM0);
        m_S1 = _parameters.getValue(ParameterId::TestS1);
        m_A1 = _parameters.getValue(ParameterId::TestA1);
        m_R1 = _parameters.getValue(ParameterId::TestR1);
        m_M1 = _parameters.getValue(ParameterId::TestM1);
        m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
        m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

    #ifdef DEBUG
        Tucuxi::Common::LoggerHelper logHelper;

        logHelper.debug("<<Input Values>>");
        logHelper.debug("m_S0: {}", m_S0);
        logHelper.debug("m_A0: {}", m_A0);
        logHelper.debug("m_S1: {}", m_S1);
        logHelper.debug("m_A1: {}", m_A1);
        logHelper.debug("m_nbPoints: {}", m_nbPoints);
        logHelper.debug("m_Int: {}", m_Int);

    #endif

        // check the inputs
        bool bOK = true;
        bOK &= checkPositiveValue(m_S0, "S0");
        bOK &= checkValidValue(m_A0, "A0");
        bOK &= checkPositiveValue(m_S1, "S1 ");
        bOK &= checkValidValue(m_A1, "A1");

        return bOK;
    }

    void computeExponentials(Eigen::VectorXd& _times) override
    {
        Eigen::VectorXd val0 = _times;
        Eigen::VectorXd val1 = _times;
        auto s1 = val0.size();
        for(int i = 0; i < s1; i++) {
            val0[i] = (1 - _times[i] * m_S0) * m_M0;
        }
        for(int j = 0; j <s1; j++){
            val1[j] = (1 - _times[j] * m_S1) * m_M1;
        }

        /*
        auto s1 = val.size();
        for(int i = 0; i < s1/2; i++) {
            val[i] = (1 - _times[i] * m_S0) * m_M0;
        }
        for(int j = s1/2; j <s1; j++){
            val[j] = (1 - _times[j] * m_S1) * m_M1;
        }*/
        setExponentials(Exponentials::P0, val0.array());
        setExponentials(Exponentials::P1, val1.array());
    }

    bool computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        Eigen::VectorXd concentrations1, concentrations2;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);
        size_t secondCompartment = static_cast<size_t>(Compartments::Second);

        // Compute concentrations
            compute(_inResiduals, concentrations1, concentrations2);

        // Return finla residual
        _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
        _outResiduals[secondCompartment] = concentrations2[m_nbPoints - 1];


        // Return concentrations of first compartment
        _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
        // Return concentrations of the second compartment
        _concentrations[secondCompartment].assign(concentrations2.data(), concentrations2.data() + concentrations2.size());
        TMP_UNUSED_PARAMETER(_isAll);

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.") && checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    }

    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        TMP_UNUSED_PARAMETER(_atTime);

        Eigen::VectorXd concentrations1, concentrations2;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);
        size_t secondCompartment = static_cast<size_t>(Compartments::Second);
        Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
        Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

        // Compute concentrations
        compute(_inResiduals, concentrations1, concentrations2);

        // Return concentrations (computation with atTime (current time))
        _concentrations[firstCompartment].push_back(concentrations1[atTime]);
        _concentrations[secondCompartment].push_back(concentrations2[atTime]);
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        // interval=0 means that it is the last cycle, so final residual = 0
        if (m_Int == 0.0) {
            concentrations1[atEndInterval] = 0;
            concentrations2[atEndInterval] = 0;
        }

        // Return final residual (computation with m_Int (interval))
        _outResiduals[firstCompartment] = concentrations1[atEndInterval];
        _outResiduals[secondCompartment] = concentrations2[atEndInterval];

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.") && checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    }


    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations, Eigen::VectorXd& _concentrations2);

    Value m_D;	/// Quantity of drug
    Value m_S0;	/// Slope of elimination of the concentration nb 0
    Value m_M0;	/// Multiplicative factor of the concentration nb 0
    Value m_A0;  /// Additional value to concentration nb 0
    Value m_R0;  /// Multiplier for the residual of the concentration nb 0
    Value m_S1;	/// Slope of elimination of the concentration nb 1
    Value m_M1;	/// Multiplicative factor of the concentration nb 1
    Value m_A1;  /// Additional value to concentration nb 1
    Value m_R1;  /// Multiplier for the residual of the concentration nb 1
    Eigen::Index m_nbPoints; /// Number measure points during interval
    Value m_Int; /// Interval (hours)

private:

    //
    //
    //
    //
    //
    // There was no space after typedef here
    //
    // I justs added
    // #include "pkmodels/multiconstanteliminationbolus.h"
    // in tests.cpp, so a compilation would use this .h file. Then the error was very clear :-)
    //
    //
    //
    typedef MultiConstantEliminationBolusCompartments Compartments;
};

inline void MultiConstantEliminationBolus::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    _concentrations1 = ((m_D + m_R0 * _inResiduals[0]) * exponentials(Exponentials::P0));
    for (int i = 0; i < _concentrations1.size(); i++) {
        _concentrations1[i] += m_A0;
        if (_concentrations1[i] < 0.0) {
            _concentrations1[i] = 0;
        }
    }


     _concentrations2 = ((m_D + m_R1 * _inResiduals[1]) * exponentials(Exponentials::P1));
     for (int i = 0; i < _concentrations2.size(); i++) {
         _concentrations2[i] += m_A1;
         if (_concentrations2[i] < 0.0) {
             _concentrations2[i] = 0;
         }
     }


}

} // namespace Core
} // namespace Tucuxi

#endif // MULTICONSTANTELIMINATIONBOLUS_H
