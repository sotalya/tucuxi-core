//@@license@@

#ifndef TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
#define TUCUXI_CORE_ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentExtraExponentials : int
{
    Ke = 0,
    Ka
};

enum class OneCompartmentExtraCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentExtraMicro : public IntakeIntervalCalculatorBase<2, OneCompartmentExtraExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMicro)
public:
    /// \brief Constructor
    OneCompartmentExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef OneCompartmentExtraExponentials Exponentials;

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

    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2);

    Value m_D;   /// Quantity of drug
    Value m_F;   /// bioavailability
    Value m_Ka;  /// Absorption rate constant
    Value m_V;   /// Volume of the compartment
    Value m_Ke;  /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Int; /// Interval time (Hours)
    Eigen::Index m_nbPoints; /// Number measure points during interval

private:
    typedef OneCompartmentExtraCompartments Compartments;
};

inline void OneCompartmentExtraMicro::compute(
        const Residuals& _inResiduals, Eigen::VectorXd& _concentrations1, Eigen::VectorXd& _concentrations2)
{
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1] + m_F * m_D / m_V;
    Concentration part2 = m_Ka * resid2 / (-m_Ka + m_Ke);

    _concentrations1 = exponentials(Exponentials::Ke) * resid1
                       + (exponentials(Exponentials::Ka) - exponentials(Exponentials::Ke)) * part2;
    _concentrations2 = resid2 * exponentials(Exponentials::Ka);
}

class OneCompartmentExtraMacro : public OneCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraMacro)
public:
    OneCompartmentExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTEXTRA_H
