#ifndef TUCUXI_CORE_RKMICHAELISMENTENENZYME_H
#define TUCUXI_CORE_RKMICHAELISMENTENENZYME_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenEnzymeCompartments : int { First = 0, Second, Enzyme };

class CalculationException : public std::exception
{
    virtual const char* what() const throw()
    {
      return "Computation value NaN in RkMichaelisMentenEnzyme calculator";
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzyme : public IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenEnzyme>
{
public:
    /// \brief Constructor
    RkMichaelisMentenEnzyme();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();


public:
    inline void derive(double _t, const std::vector<double> &_c, std::vector<double>& _dcdt)
    {
        //auto c0 = _c[0] / 1000.0 * m_V;
        //auto cp = c0 / m_V;
        // We get concentration 0 in mg/l instead of ug/l
        auto cp = _c[0] / 1000.0;
        _dcdt[0] = m_Ka * _c[1] - m_Vmax * cp * m_AllmCL / (m_Km + cp) * _c[2]; // / m_V;
        auto ktt = m_ktr * _t;
        if (m_MTT == 0.0) {
            _dcdt[1] = -m_Ka * _c[1];
        }
        else {
            _dcdt[1] = -m_Ka * _c[1] + std::exp(m_cumul + m_NN * std::log(ktt) - ktt);
        }
        _dcdt[2] = m_Kenz * (1 + m_Emax * cp / (m_ECmid + cp)) - m_Kenz * _c[2];

        // Here we modify the derivative on the central compartment, in order
        // to consider it in ug/l     (1000.0 for ug, and /m_V for concentration)
        _dcdt[0] *= 1000.0 / m_V;
        if (std::isnan(_dcdt[0])) {
            throw CalculationException();
        }
        if (std::isnan(_dcdt[1])) {
            throw CalculationException();
        }
        if (std::isnan(_dcdt[2])) {
            throw CalculationException();
        }
        //assert(!std::isnan(_dcdt[0]));
        //assert(!std::isnan(_dcdt[1]));
        //assert(!std::isnan(_dcdt[2]));
    }

protected:


    Value m_D;	/// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Km;
    Value m_Vmax;
    Value m_AllmCL; /// Allometric clearance
    Value m_Tinf;

    Value m_Kenz;
    Value m_Emax;
    Value m_ECmid;
    Value m_EDmid;
    Value m_DoseMid;
    Value m_Fmax;
    Value m_NN;
    Value m_MTT;

    Value m_cumul;
    Value m_ktr;


private:
    typedef RkMichaelisMentenEnzymeCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeExtra : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeExtra)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeExtra();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        if (m_D > 0.0) {
            if (_inResiduals[2] == 0.0) {
                // First dose, we need an induction value
                _concentrations[2] = 1.0;
            }
            else {
                // Following dose
                //_concentrations[2] = 0.0;
                _concentrations[2] = _inResiduals[2];
            }
        }
        else {
            _concentrations[2] = _inResiduals[2];
        }
        // TODO : Check if this induction should be set to 1 everytime,
        //        or only for the first dose
    }

protected:

};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeBolus : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeBolus)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeBolus();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D * m_F / m_V;
        _concentrations[1] = _inResiduals[1];
        if (m_D > 0.0) {
            _concentrations[2] = _inResiduals[2] + 1.0;
        }
        else {
            _concentrations[2] = _inResiduals[2];
        }
    }

protected:

};

/*
/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenEnzymeInfusion : public RkMichaelisMentenEnzyme
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenEnzymeInfusion)
public:
    /// \brief Constructor
    RkMichaelisMentenEnzymeInfusion();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];

    }

protected:

};
*/


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTENENZYME_H
