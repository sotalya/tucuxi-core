//@@license@@

#ifndef TUCUXI_CORE_ONECOMPARTMENTEXTRALAG_H
#define TUCUXI_CORE_ONECOMPARTMENTEXTRALAG_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class OneCompartmentExtraLagExponentials : int
{
    Ke = 0,
    Ka,
    KePost,
    KaPost
};

enum class OneCompartmentExtraLagCompartments : int
{
    First = 0,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the one compartment extravascular algorithm
/// \sa IntakeIntervalCalculator
class OneCompartmentExtraLagMicro : public IntakeIntervalCalculatorBase<2, OneCompartmentExtraLagExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraLagMicro)
public:
    /// \brief Constructor
    OneCompartmentExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef OneCompartmentExtraLagExponentials Exponentials;

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

    Value m_D;    /// Quantity of drug
    Value m_F;    /// bioavailability
    Value m_Ka;   /// Absorption rate constant
    Value m_V;    /// Volume of the compartment
    Value m_Ke;   /// Elimination constant rate = Cl/V where Cl is the clearance and V is the volume of the compartment
    Value m_Tlag; /// Lag time (in hours)
    Value m_Int;  /// Interval time (Hours)
    Eigen::Index m_nbPoints;  /// Number measure points during interval
    Eigen::Index m_nbPoints0; /// Number of points during lag time
    Eigen::Index m_nbPoints1; /// Number of points after lag time


private:
    typedef OneCompartmentExtraLagCompartments Compartments;
};


class OneCompartmentExtraLagMacro : public OneCompartmentExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(OneCompartmentExtraLagMacro)
public:
    OneCompartmentExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_ONECOMPARTMENTEXTRALAG_H
