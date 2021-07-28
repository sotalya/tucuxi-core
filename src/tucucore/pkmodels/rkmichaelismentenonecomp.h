#ifndef TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H
#define TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenOneCompCompartments : int { First = 0, Second };

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneComp : public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneComp>
{
public:
    /// \brief Constructor
    RkMichaelisMentenOneComp();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();


public:
    inline void derive(double _t, const std::vector<double> &_c, std::vector<double>& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ka * _c[1] - m_Vmax * _c[0] / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];
    }

    inline void addFixedValue(double _t, std::vector<double>& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:


    Value m_D;	/// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_Ka; /// Absorption rate constant
    Value m_V;  /// Volume of the compartment
    Value m_Km;
    Value m_Vmax;
    Value m_Tinf;


private:
    typedef RkMichaelisMentenOneCompCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompExtra : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompExtra)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompExtra();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1] + m_D / m_V * m_F;
    }

protected:

};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompBolus : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompBolus)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompBolus();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations (const Residuals& _inResiduals, std::vector<double> &_concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D / m_V * m_F;
        _concentrations[1] = _inResiduals[1];
    }

protected:

};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompInfusion : public RkMichaelisMentenOneComp
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompInfusion)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompInfusion();

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



} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKMICHAELISMENTENONECOMP_H
