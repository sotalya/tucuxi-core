//@@license@@

#ifndef TUCUXI_CORE_RKMICHAELISMENTENONECOMPVMAXAMOUNT_H
#define TUCUXI_CORE_RKMICHAELISMENTENONECOMPVMAXAMOUNT_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkMichaelisMentenOneCompVmaxAmountCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompVmaxAmount : public IntakeIntervalCalculatorRK4Base<2, RkMichaelisMentenOneCompVmaxAmount>
{

public:
    /// \brief Constructor
    RkMichaelisMentenOneCompVmaxAmount();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ka * _c[1] - m_Vmax * _c[0] / m_V / (m_Km + _c[0]);
        _dcdt[1] = -m_Ka * _c[1];
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        FINAL_UNUSED_PARAMETER(_t);
        FINAL_UNUSED_PARAMETER(_concentrations);
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{NAN};  /// bioavailability
    Value m_Ka{NAN}; /// Absorption rate constant
    Value m_V{NAN};  /// Volume of the compartment
    Value m_Km{NAN};
    Value m_Vmax{NAN};
    Value m_Tinf{NAN};

private:
    typedef RkMichaelisMentenOneCompVmaxAmountCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompVmaxAmountExtra : public RkMichaelisMentenOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompVmaxAmountExtra)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompVmaxAmountExtra();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1] + m_D / m_V * m_F;
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompVmaxAmountBolus : public RkMichaelisMentenOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompVmaxAmountBolus)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompVmaxAmountBolus();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D / m_V * m_F;
        _concentrations[1] = _inResiduals[1];
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkMichaelisMentenOneCompVmaxAmountInfusion : public RkMichaelisMentenOneCompVmaxAmount
{
    INTAKEINTERVALCALCULATOR_UTILS(RkMichaelisMentenOneCompVmaxAmountInfusion)
public:
    /// \brief Constructor
    RkMichaelisMentenOneCompVmaxAmountInfusion();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
    }
};



} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RKMICHAELISMENTENONECOMPVMAXAMOUNT_H
