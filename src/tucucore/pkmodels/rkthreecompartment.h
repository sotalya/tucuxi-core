//@@license@@

#ifndef TUCUXI_CORE_RKTHREECOMPARTMENT_H
#define TUCUXI_CORE_RKTHREECOMPARTMENT_H

#include "tucucore/intakeintervalcalculatorrk4.h"

namespace Tucuxi {
namespace Core {


enum class RkThreeCompartmentCompartments : int
{
    First = 0,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartment : public IntakeIntervalCalculatorRK4Base<4, RkThreeCompartment>
{

public:
    /// \brief Constructor
    RkThreeCompartment();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    inline void derive(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[3] + m_K21 * _c[1] - m_K12 * _c[0] + m_K31 * _c[2] - m_K13 * _c[0] - m_Ke * _c[0];
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = m_K13 * _c[0] - m_K31 * _c[2];
        _dcdt[3] = -m_Ka * _c[3];

        if (m_isInfusion) {
            if (_t < m_TinfLow) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }

    inline void deriveAtPotentialInfusionStop(double _t, const Compartments_t& _c, Compartments_t& _dcdt)
    {
        // This version considers VMax to be concentration/time
        _dcdt[0] = m_Ka * _c[3] + m_K21 * _c[1] - m_K12 * _c[0] + m_K31 * _c[2] - m_K13 * _c[0] - m_Ke * _c[0];
        _dcdt[1] = m_K12 * _c[0] - m_K21 * _c[1];
        _dcdt[2] = m_K13 * _c[0] - m_K31 * _c[2];
        _dcdt[3] = -m_Ka * _c[3];

        if (m_isInfusion) {
            if (_t < m_TinfHigh) {
                _dcdt[0] += m_infusionRate;
            }
        }
    }

    inline void addFixedValue(double _t, Compartments_t& _concentrations)
    {
        if (m_isWithLag) {
            if ((!m_delivered) && (_t >= m_Tlag)) {
                _concentrations[3] += m_D / m_V1 * m_F;
                m_delivered = true;
            }
        }
    }

protected:
    Value m_D{NAN};  /// Quantity of drug
    Value m_F{1.0};  /// bioavailability
    Value m_Ka{0.0}; /// Absorption rate constant
    Value m_V1{NAN}; /// Volume of the compartment
    Value m_Ke{NAN};
    Value m_K12{NAN};
    Value m_K21{NAN};
    Value m_K13{NAN};
    Value m_K31{NAN};
    Value m_Tinf{NAN};
    Value m_TinfLow{NAN};
    Value m_TinfHigh{NAN};
    Value m_Tlag{NAN};
    Value m_infusionRate{0};
    bool m_delivered{false};
    bool m_isInfusion{false};
    bool m_isWithLag{false};

private:
    typedef RkThreeCompartmentCompartments Compartments;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentExtraMicro : public RkThreeCompartment
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentExtraMicro)
public:
    /// \brief Constructor
    RkThreeCompartmentExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
        _concentrations[3] = _inResiduals[3] + m_D / m_V1 * m_F;
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentExtraLagMicro : public RkThreeCompartment
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentExtraLagMicro)
public:
    /// \brief Constructor
    RkThreeCompartmentExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
        _concentrations[3] = _inResiduals[3];
        // Do not forget to reinitialize the flag for delivery of the drug
        m_delivered = false;
    }
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentBolusMicro : public RkThreeCompartment
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentBolusMicro)
public:
    /// \brief Constructor
    RkThreeCompartmentBolusMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        _concentrations[0] = _inResiduals[0] + m_D / m_V1;
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
        _concentrations[3] = _inResiduals[3];
    }
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentInfusionMicro : public RkThreeCompartment
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentInfusionMicro)
public:
    /// \brief Constructor
    RkThreeCompartmentInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void initConcentrations(const Residuals& _inResiduals, MultiCompConcentration& _concentrations) override
    {
        m_infusionRate = m_D / m_V1 / m_Tinf;
        _concentrations[0] = _inResiduals[0];
        _concentrations[1] = _inResiduals[1];
        _concentrations[2] = _inResiduals[2];
        _concentrations[3] = _inResiduals[3];
    }
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentExtraMacro : public RkThreeCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentExtraMacro)
public:
    /// \brief Constructor
    RkThreeCompartmentExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentExtraLagMacro : public RkThreeCompartmentExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentExtraLagMacro)
public:
    /// \brief Constructor
    RkThreeCompartmentExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentBolusMacro : public RkThreeCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentBolusMacro)
public:
    /// \brief Constructor
    RkThreeCompartmentBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class RkThreeCompartmentInfusionMacro : public RkThreeCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(RkThreeCompartmentInfusionMacro)
public:
    /// \brief Constructor
    RkThreeCompartmentInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_RKTHREECOMPARTMENT_H
