/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
#define TUCUXI_CORE_ONECOMPARTMENTBOLUS_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentBolusExponentials : int
{
    Ke
};

enum class OneCompartmentBolusCompartments : int
{
    First
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment bolus algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentBolusMicro : public IntakeIntervalCalculatorBase<1, OneCompartmentBolusExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentBolusMicro)
public:
    /// \brief Constructor
    OneCompartmentBolusMicro();

    typedef OneCompartmentBolusExponentials Exponentials;


    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;

    void computeExponentials(Eigen::VectorXd& _times) override;

    bool computeConcentrations(
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override;

    bool computeConcentration(
            const Value& _atTime,
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) override;

    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

    Value m_D;  /// Quantity of drug
    Value m_V;  /// Volume of the compartment
    Value m_Ke; /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Eigen::Index m_nbPoints; /// Number measure points during interval
    Value m_Int;             /// Interval (hours)

private:
    typedef OneCompartmentBolusCompartments Compartments;
};

inline void OneCompartmentBolusMicro::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{
    _concentrations = (m_D / m_V + _inResiduals[0]) * exponentials(Exponentials::Ke);
}

class OneCompartmentBolusMacro : public OneCompartmentBolusMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentBolusMacro)
public:
    OneCompartmentBolusMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTBOLUS_H
