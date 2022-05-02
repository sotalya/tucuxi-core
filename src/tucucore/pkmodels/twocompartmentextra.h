//@@license@@

#ifndef TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H
#define TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class TwoCompartmentExtraExponentials : int
{
    Alpha,
    Beta,
    Ka
};

enum class TwoCompartmentExtraCompartments : int
{
    First,
    Second,
    Third
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment extra algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentExtraMicro : public IntakeIntervalCalculatorBase<3, TwoCompartmentExtraExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraMicro)
public:
    /// \brief Constructor
    TwoCompartmentExtraMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef TwoCompartmentExtraExponentials Exponentials;

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
    Value m_K12;             /// Q/V1
    Value m_K21;             /// Q/V2
    Value m_RootK;           /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha;           /// (sumK + root)/2
    Value m_Beta;            /// (sumK - root)/2
    Eigen::Index m_nbPoints; /// number measure points during interval
    Value m_Int;             /// Interval (hours)

private:
    typedef TwoCompartmentExtraCompartments Compartments;
};

inline bool TwoCompartmentExtraMicro::compute(
        const Residuals& _inResiduals,
        Eigen::VectorXd& _concentrations1,
        Eigen::VectorXd& _concentrations2,
        Eigen::VectorXd& _concentrations3)
{
    Value A;       // NOLINT(readability-identifier-naming)
    Value B;       // NOLINT(readability-identifier-naming)
    Value C;       // NOLINT(readability-identifier-naming)
    Value divider; // NOLINT(readability-identifier-naming)
    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];
    Concentration resid3 = _inResiduals[2] + (m_F * m_D / m_V1);
    Value sumResid13 = resid1 + resid3;
    Value sumK12K21 = m_K12 + m_K21;
    Value sumK21Ke = m_K21 + m_Ke;
    Value diffK21Ka = m_K21 - m_Ka;
    Value diffK21Ke = m_K21 - m_Ke;
    Value powDiffK21Ke = std::pow(diffK21Ke, 2);

    // For compartment1, calculate A, B, C and divider
    A = std::pow(m_K12, 3) * m_Ka * resid1
        + diffK21Ka
                  * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13)
                     + ((m_Ka - m_Ke) * (m_Ke * resid1 - m_K21 * (resid1 + 2 * resid2))
                        + m_Ka * (-m_K21 + m_Ke) * resid3)
                               * m_RootK)
        + std::pow(m_K12, 2)
                  * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
                     + m_Ka * (-m_Ka * sumResid13 + resid1 * (3 * m_Ke + m_RootK)))
        + m_K12
                  * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3)
                     - m_K21
                               * (2 * std::pow(m_Ka, 2) * sumResid13 - 2 * m_Ka * m_Ke * sumResid13
                                  + m_Ka * (2 * resid2 + resid3) * m_RootK + m_Ke * resid1 * (2 * m_Ke + m_RootK))
                     - m_Ka * (m_Ka * sumResid13 * (2 * m_Ke + m_RootK) - m_Ke * resid1 * (3 * m_Ke + 2 * m_RootK)));

    B = std::pow(m_K12, 3) * m_Ka * resid1
        + diffK21Ka
                  * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13)
                     + ((m_Ka - m_Ke) * (-m_Ke * resid1 + m_K21 * (resid1 + 2 * resid2)) + m_Ka * diffK21Ke * resid3)
                               * m_RootK)
        + std::pow(m_K12, 2)
                  * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
                     - m_Ka * (m_Ka * sumResid13 + resid1 * (-3 * m_Ke + m_RootK)))
        + m_K12
                  * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3)
                     + m_Ka * (m_Ke * resid1 * (3 * m_Ke - 2 * m_RootK) - m_Ka * sumResid13 * (2 * m_Ke - m_RootK))
                     + m_K21
                               * (-2 * std::pow(m_Ka, 2) * sumResid13 + 2 * m_Ka * m_Ke * sumResid13
                                  + m_Ka * (2 * resid2 + resid3) * m_RootK + m_Ke * resid1 * (-2 * m_Ke + m_RootK)));

    C = -2 * diffK21Ka * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) * resid3;

    divider = std::pow((sumK12K21 - 2 * m_Ka + m_Ke) * m_RootK, 2)
              - std::pow(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2);

    if (!checkCondition(divider != 0.0, "Dividing by zero.")) {
        return false;
    }

    // Calculate concentrations of compartment 1
    _concentrations1 = -2
                       * (B * exponentials(Exponentials::Beta) + A * exponentials(Exponentials::Alpha)
                          + C * exponentials(Exponentials::Ka))
                       / divider;

    // For compartment1, calculate A, B, C and divider
    A = -(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke)
                * (diffK21Ka * (m_Ka - m_Ke) * resid2 + m_K12 * m_Ka * (resid2 + resid3))
        + ((m_K12 * m_Ka + diffK21Ka * (m_Ka - m_Ke)) * (2 * m_K12 * resid1 + (m_K12 - diffK21Ke) * resid2)
           + m_K12 * m_Ka * (sumK12K21 - 2 * m_Ka + m_Ke) * resid3)
                  * m_RootK;

    B = -(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke)
                * (diffK21Ka * (m_Ka - m_Ke) * resid2 + m_K12 * m_Ka * (resid2 + resid3))
        - ((m_K12 * m_Ka + diffK21Ka * (m_Ka - m_Ke)) * (2 * m_K12 * resid1 + (m_K12 - diffK21Ke) * resid2)
           + m_K12 * m_Ka * (sumK12K21 - 2 * m_Ka + m_Ke) * resid3)
                  * m_RootK;

    C = 2 * m_K12 * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) * resid3;

    divider = -std::pow(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2)
              + std::pow(sumK12K21 - 2 * m_Ka + m_Ke, 2) * std::pow(m_RootK, 2);

    if (!checkCondition(divider != 0.0, "Dividing by zero.")) {
        return false;
    }

    // Calculate concentrations of compartment 2 and 3
    _concentrations2 = 2
                       * (B * exponentials(Exponentials::Beta) + A * exponentials(Exponentials::Alpha)
                          + C * exponentials(Exponentials::Ka))
                       / divider;
    _concentrations3 = exponentials(Exponentials::Ka) * resid3;

    return true;
}


class TwoCompartmentExtraMacro : public TwoCompartmentExtraMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentExtraMacro)
public:
    TwoCompartmentExtraMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TWOCOMPARTMENTEXTRA_H
