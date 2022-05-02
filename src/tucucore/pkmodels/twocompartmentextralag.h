//@@license@@

#ifndef TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H
#define TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class TwoCompartmentExtraLagExponentials : int
{
    Alpha,
    Beta,
    Ka,
    AlphaPost,
    BetaPost,
    KaPost
};

enum class TwoCompartmentExtraLagCompartments : int
{
    First,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment extra algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentExtraLagMicro : public IntakeIntervalCalculatorBase<3, TwoCompartmentExtraLagExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraLagMicro)
public:
    /// \brief Constructor
    TwoCompartmentExtraLagMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef TwoCompartmentExtraLagExponentials Exponentials;

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

    bool compute(
            const Residuals& _inResiduals,
            Eigen::VectorXd& _concentrations1,
            Eigen::VectorXd& _concentrations2,
            Eigen::VectorXd& _concentrations3);

    Value m_D;  /// Quantity of drug
    Value m_F;  /// bioavailability
    Value m_V1; /// Volume1
    Value m_Ka; /// Absorption rate constant
    Value m_Ke; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12;              /// Q/V1
    Value m_K21;              /// Q/V2
    Value m_Tlag;             /// Lag time (in hours)
    Value m_RootK;            /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha;            /// (sumK + root)/2
    Value m_Beta;             /// (sumK - root)/2
    Value m_Int;              /// Interval (hours)
    Eigen::Index m_nbPoints;  /// number measure points during interval
    Eigen::Index m_nbPoints0; /// Number of points during lag time
    Eigen::Index m_nbPoints1; /// Number of points after lag time

private:
    typedef TwoCompartmentExtraLagCompartments Compartments;
};



class TwoCompartmentExtraLagMacro : public TwoCompartmentExtraLagMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraLagMacro)
public:
    TwoCompartmentExtraLagMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TWOCOMPARTMENTEXTRALAG_H
