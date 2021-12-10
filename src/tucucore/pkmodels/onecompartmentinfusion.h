/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
#define TUCUXI_CORE_ONECOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentInfusionExponentials : int
{
    Ke
};

enum class OneCompartmentInfusionCompartments : int
{
    First
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<1, OneCompartmentInfusionExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentInfusionMicro)
public:
    /// \brief Constructor
    OneCompartmentInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef OneCompartmentInfusionExponentials Exponentials;

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void computeExponentials(Eigen::VectorXd& _times) override;

    bool computeConcentrations(
            const Residuals& _inResiduals,
            bool _isAll,
            std::vector<Concentrations>& _concentrations,
            Residuals& _outResiduals) override;

    bool computeConcentration(
            const Value& _atTime,
            const Residuals& _inResiduals,
            bool _isAll,
            std::vector<Concentrations>& _concentrations,
            Residuals& _outResiduals) override;

    void compute(const Residuals& _inResiduals, int _forceSize, Eigen::VectorXd& _concentrations);


    Value m_D;    /// Quantity of drug
    Value m_Cl;   /// Clearance
    Value m_V;    /// Volume of the compartment
    Value m_Ke;   /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Tinf; /// Infusion time (hours)
    Value m_Int;  /// Interval (hours)
    Eigen::Index m_nbPoints; /// number measure points during interval

private:
    typedef OneCompartmentInfusionCompartments Compartments;
};

inline void OneCompartmentInfusionMicro::compute(
        const Residuals& _inResiduals, int _forceSize, Eigen::VectorXd& _concentrations)
{
    //    Concentration part1 = m_D / (m_Tinf * m_Cl);
    Concentration part1 = m_D / (m_Tinf * m_Ke * m_V);

    // Calcaulate concentrations
    _concentrations = Eigen::VectorXd::Constant(exponentials(Exponentials::Ke).size(), _inResiduals[0]);
    _concentrations = _concentrations.cwiseProduct(exponentials(Exponentials::Ke));

    if (_forceSize != 0) {
        _concentrations.head(_forceSize) =
                _concentrations.head(_forceSize)
                + part1 * (1.0 - exponentials(Exponentials::Ke).head(_forceSize).array()).matrix();
    }

    int therest = static_cast<int>(_concentrations.size() - _forceSize);
    _concentrations.tail(therest) = _concentrations.tail(therest)
                                    + part1 * (exp(m_Ke * m_Tinf) - 1) * exponentials(Exponentials::Ke).tail(therest);
}

class OneCompartmentInfusionMacro : public OneCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentInfusionMacro)
public:
    OneCompartmentInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTINFUSION_H
