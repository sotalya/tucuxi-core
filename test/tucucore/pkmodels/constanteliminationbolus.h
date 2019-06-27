/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef LINEARBOLUS_H
#define LINEARBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

enum class ConstantEliminationBolusExponentials : int { P };
enum class ConstantEliminationBolusCompartments : int { First };

/// \ingroup TucuCore
/// \brief Intake interval calculator for tests. It is a bolus with a linear slope
///
/// 3 parameters:
///     - A
///     - S
///     - R
/// The equation is the following:
/// C(t) = max(0.0 , (D + residual * R) * ( 1 - t * S) * M + A)
/// It allows to test other components with a simple equation:
///     - Easy to calculate AUC
///     - Easy to calculate residual
///     - Easy to calculate percentiles depending on variability of A
///
/// When using R=0, S=0 and A=0, the concentration is simply the dose
///
class ConstantEliminationBolus : public IntakeIntervalCalculatorBase<1, ConstantEliminationBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(ConstantEliminationBolus)
public:
    /// \brief Constructor
    ConstantEliminationBolus() : IntakeIntervalCalculatorBase<1, ConstantEliminationBolusExponentials> (new PertinentTimesCalculatorStandard())
    {

    }

    typedef ConstantEliminationBolusExponentials Exponentials;


    static std::vector<std::string> getParametersId()
    {
        return {"TestA", "TestS", "TestR", "TestM"};
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {

        if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
            return false;
        }

        m_D = _intakeEvent.getDose() * 1000;
        m_S = _parameters.getValue(ParameterId::TestS);
        m_A = _parameters.getValue(ParameterId::TestA);
        m_R = _parameters.getValue(ParameterId::TestR);
        m_M = _parameters.getValue(ParameterId::TestM);
        m_NbPoints = _intakeEvent.getNbPoints();
        m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

    #ifdef DEBUG
        Tucuxi::Common::LoggerHelper logHelper;

        logHelper.debug("<<Input Values>>");
        logHelper.debug("m_S: {}", m_S);
        logHelper.debug("m_A: {}", m_A);
        logHelper.debug("m_NbPoints: {}", m_NbPoints);
        logHelper.debug("m_Int: {}", m_Int);
    #endif

        // check the inputs
        bool bOK = true;
        bOK &= checkValue(!std::isnan(m_S), "The dose is NaN.");
        bOK &= checkValue(!std::isinf(m_S), "The dose is Inf.");
        bOK &= checkValue(m_S >= 0, "The negative slope is not greater than zero.");
        bOK &= checkValue(!std::isnan(m_A), "The V is NaN.");
        bOK &= checkValue(!std::isinf(m_A), "The V is Inf.");

        return bOK;
    }

    void computeExponentials(Eigen::VectorXd& _times) override
    {
        Eigen::VectorXd val = _times;
        int s = val.size();
        for(int i = 0; i < s; i++) {
            val[i] = (1 - _times[i] * m_S) * m_M;
        }
        setExponentials(Exponentials::P, val.array());
    }

    bool computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        Eigen::VectorXd concentrations;
        int firstCompartment = static_cast<int>(Compartments::First);


        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return finla residual
        _outResiduals[firstCompartment] = concentrations[m_NbPoints - 1];

        // Return concentraions of first compartment
        _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        return checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }

    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        TMP_UNUSED_PARAMETER(_atTime);

        Eigen::VectorXd concentrations;
        int firstCompartment = static_cast<int>(Compartments::First);
        int atTime = static_cast<int>(SingleConcentrations::AtTime);
        int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return concentrations (computation with atTime (current time))
        _concentrations[firstCompartment].push_back(concentrations[atTime]);
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        // interval=0 means that it is the last cycle, so final residual = 0
        if (m_Int == 0.0) {
            concentrations[atEndInterval] = 0;
        }

        // Return final residual (computation with m_Int (interval))
        _outResiduals[firstCompartment] = concentrations[atEndInterval];

        return checkValue(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }


    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

    Value m_D;	/// Quantity of drug
    Value m_S;	/// Slope of elimination
    Value m_M;	/// Multiplicative factor of the concentration
    Value m_A;  /// Additional value to concentration
    Value m_R;  /// Multiplier for the residual
    int m_NbPoints; /// Number measure points during interval
    Value m_Int; /// Interval (hours)

private:
    typedef ConstantEliminationBolusCompartments Compartments;
};

inline void ConstantEliminationBolus::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{
    _concentrations = (m_D + m_R * _inResiduals[0]) * exponentials(Exponentials::P);
    for (int i = 0; i < _concentrations.size(); i++) {
        _concentrations[i] += m_A;
        if (_concentrations[i] < 0.0) {
            _concentrations[i] = 0;
        }
    }
}

} // namespace Core
} // namespace Tucuxi


#endif // LINEARBOLUS_H
