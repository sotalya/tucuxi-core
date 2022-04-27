//@@lisence@@

#ifndef TUCUXI_CORE_RK4TWOCOMPARTMENTEXTRALAG_H
#define TUCUXI_CORE_RK4TWOCOMPARTMENTEXTRALAG_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {

enum class RK4TwoCompartmentExtraLagCompartments : int
{
    First = 0,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RK4TwoCompartmentExtraLagMicro : public IntakeIntervalCalculatorRK4Base<3, RK4TwoCompartmentExtraLagMicro>
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentExtraLagMicro)
public:
    /// \brief Constructor
    RK4TwoCompartmentExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const MultiCompConcentration& _c, MultiCompConcentration& _dcdt)
    {
        FINAL_UNUSED_PARAMETER(_t);
        _dcdt[0] = m_Ka * _c[2] - m_Ke * _c[0] - m_K12 * _c[0] + m_K21 * _c[1];
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = -m_Ka * _c[2];
    }

    inline void addFixedValue(double _t, MultiCompConcentration& _concentrations)
    {
        if ((!m_delivered) && (_t >= m_Tlag)) {
            _concentrations[2] += m_D / m_V1 * m_F;
            m_delivered = true;
        }
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];

        // Do not forget to reinitialize the flag for delivery of the drug
        m_delivered = false;
    }

    Value m_D;  /// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_V1; /// Volume1
    Value m_Ka; /// Absorption rate constant
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12;  /// Q/V1
    Value m_K21;  /// Q/V2
    Value m_Tlag; /// Lag time (in hours)

    bool m_delivered{false};

private:
    typedef RK4TwoCompartmentExtraLagCompartments Compartments;
};



class RK4TwoCompartmentExtraLagMacro : public RK4TwoCompartmentExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RK4TwoCompartmentExtraLagMacro)
public:
    RK4TwoCompartmentExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_RK4TWOCOMPARTMENTEXTRALAG_H
