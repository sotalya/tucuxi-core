/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_TWOCOMPARTMENTINFUSION_H
#define TUCUXI_CORE_TWOCOMPARTMENTINFUSION_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

namespace Tucuxi {
namespace Core {

enum class TwoCompartmentInfusionExponentials : int
{
    Alpha,
    Beta,
    AlphaInf,
    BetaInf,
    BetaInf2,
    Root,
    AlphaPostInf,
    BetaPostInf
};

enum class TwoCompartmentInfusionCompartments : int
{
    First,
    Second
};

/// \ingroup TucuCore
/// \brief Intake interval calculator for the two compartment infusion algorithm
/// \sa IntakeIntervalCalculator
class TwoCompartmentInfusionMicro : public IntakeIntervalCalculatorBase<2, TwoCompartmentInfusionExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentInfusionMicro)
public:
    /// \brief Constructor
    TwoCompartmentInfusionMicro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

    typedef TwoCompartmentInfusionExponentials Exponentials;

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

    void compute(
            const Residuals& _inResiduals,
            int _forceSize,
            Eigen::VectorXd& _concentrations1,
            Eigen::VectorXd& _concentrations2);

    Value m_D{NAN};  /// Quantity of drug
    Value m_V1{NAN}; /// Volume of the compartment 1
    Value m_Ke{
            NAN}; /// Elimination constant rate = Cl/V1 where Cl is the clearance and V1 is the volume of the compartment 1
    Value m_K12{NAN};           /// Q/V1
    Value m_K21{NAN};           /// Q/V2
    Value m_SumK{NAN};          /// Ke+K12+K21
    Value m_RootK{NAN};         /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Divider{NAN};       /// sqrt(sumK*sumK - 4*K21*Ke)
    Value m_Alpha{NAN};         /// (sumK + root)/2
    Value m_Beta{NAN};          /// (sumK - root)/2
    Value m_Tinf{NAN};          /// Infusion time (Hours)
    Value m_Int{NAN};           /// Interval (Hours)
    Eigen::Index m_nbPoints{0}; /// number measure points during interval

    bool m_timeMaxHigherThanTinf{true};
    // Only used for debugging purpose
    // Value m_V2;
    // Value m_Q;
    // Value m_Cl;

private:
    typedef TwoCompartmentInfusionCompartments Compartments;
};

inline void TwoCompartmentInfusionMicro::compute(
        const Residuals& _inResiduals,
        int _forceSize,
        Eigen::VectorXd& _concentrations1,
        Eigen::VectorXd& _concentrations2)
{
    const Eigen::VectorXd& alphaLogV = exponentials(Exponentials::Alpha);
    const Eigen::VectorXd& betaLogV = exponentials(Exponentials::Beta);
    const Eigen::VectorXd& alphaInfLogV = exponentials(Exponentials::AlphaInf);
    const Eigen::VectorXd& betaInfLogV = exponentials(Exponentials::BetaInf);
    const Eigen::VectorXd& betaInf2LogV = exponentials(Exponentials::BetaInf2);
    const Eigen::VectorXd& rootLogV = exponentials(Exponentials::Root);
    const Eigen::VectorXd& alphaPostInfLogV = exponentials(Exponentials::AlphaPostInf);
    const Eigen::VectorXd& betaPostInfLogV = exponentials(Exponentials::BetaPostInf);

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];

    Value deltaD = (m_D / m_V1) / m_Tinf;

    Concentration residInf1 = (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K21
                               * (std::exp(-m_Beta * m_Tinf) * (-m_K12 - m_K21 + m_Ke - m_RootK)
                                  + std::exp(-2 * m_Beta * m_Tinf) * (m_K12 + m_K21 - m_Ke + m_RootK)
                                  + std::exp(m_RootK * m_Tinf - m_Alpha * m_Tinf) * (m_K12 + m_K21 - m_Ke - m_RootK)
                                  + std::exp(-m_Alpha * m_Tinf - m_Beta * m_Tinf) * (-m_K12 - m_K21 + m_Ke + m_RootK)))
                              / m_Divider;

    Concentration residInf2 =
            (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K12
             * (std::exp(-m_Beta * m_Tinf) * (-m_SumK - m_RootK) + std::exp(-2 * m_Beta * m_Tinf) * (m_SumK + m_RootK)
                + std::exp(m_RootK * m_Tinf - m_Alpha * m_Tinf) * (m_SumK - m_RootK)
                + std::exp(-m_Alpha * m_Tinf - m_Beta * m_Tinf) * (-m_SumK + m_RootK)))
            / m_Divider;

    Value A =
            ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2); // NOLINT(readability-identifier-naming)
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1)
              + (2 * m_K21 * resid2); // NOLINT(readability-identifier-naming)
    Value A2 = (-2 * m_K12 * resid1)
               + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2); // NOLINT(readability-identifier-naming)
    Value BB2 =
            (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2); // NOLINT(readability-identifier-naming)
    Value AInf = -m_K12 - m_K21 + m_Ke - m_RootK;                               // NOLINT(readability-identifier-naming)
    Value BInf = m_K12 + m_K21 - m_Ke - m_RootK;                                // NOLINT(readability-identifier-naming)
    Value BPostInf = (-m_K12 + m_K21 - m_Ke + m_RootK) * residInf1
                     + 2 * m_K21 * residInf2; // NOLINT(readability-identifier-naming)
    Value APostInf = (m_K12 - m_K21 + m_Ke + m_RootK) * residInf1
                     - 2 * m_K21 * residInf2; // NOLINT(readability-identifier-naming)
    Value B2PostInf = 2 * m_K12 * residInf1
                      + (m_K12 - m_K21 + m_Ke + m_RootK) * residInf2; // NOLINT(readability-identifier-naming)
    Value A2PostInf = -2 * m_K12 * residInf1
                      + (-m_K12 + m_K21 - m_Ke + m_RootK) * residInf2; // NOLINT(readability-identifier-naming)

    // Calculate concentrations for comp1 and comp2
    // This only involves the initial residuals
    _concentrations1 = ((A * alphaLogV) + (B * betaLogV)) / (2 * m_RootK);
    _concentrations2 = ((A2 * alphaLogV) + (BB2 * betaLogV)) / (2 * m_RootK);

    // During infusion
    if (_forceSize != 0) {
        Eigen::ArrayXd p1p1 = (2 * deltaD * m_K21 * betaInfLogV.head(_forceSize)).array();
        Eigen::ArrayXd p1p2 = AInf * (betaLogV.head(_forceSize) - betaInf2LogV.head(_forceSize));
        Eigen::ArrayXd p1p3 = (BInf
                               * (rootLogV.head(_forceSize).cwiseQuotient(alphaInfLogV.head(_forceSize))
                                  - alphaLogV.head(_forceSize).cwiseQuotient(betaInfLogV.head(_forceSize))))
                                      .array();
        Eigen::ArrayXd p2p1 = (2 * deltaD * m_K12 * betaInfLogV.head(_forceSize)).array();
        Eigen::ArrayXd p2p2 =
                betaLogV.head(_forceSize) * (-m_SumK - m_RootK) + betaInf2LogV.head(_forceSize) * (m_SumK + m_RootK)
                + rootLogV.head(_forceSize).cwiseQuotient(alphaInfLogV.head(_forceSize)) * (m_SumK - m_RootK)
                + alphaLogV.head(_forceSize).cwiseQuotient(betaInfLogV.head(_forceSize)) * (-m_SumK + m_RootK);

        // Add the concentration part related to the dose during infusion
        _concentrations1.head(_forceSize) += ((p1p1 * (p1p2 + p1p3)) / m_Divider).matrix();
        _concentrations2.head(_forceSize) += ((p2p1 * p2p2) / m_Divider).matrix();
    }

    if (m_timeMaxHigherThanTinf) {
        // After infusion
        int nbPostInfusionPoints = static_cast<int>(_concentrations1.size() - _forceSize);

        // Add the concentration part related to the dose after infusion
        _concentrations1.tail(nbPostInfusionPoints) += (APostInf * alphaPostInfLogV.tail(nbPostInfusionPoints)
                                                        + BPostInf * betaPostInfLogV.tail(nbPostInfusionPoints))
                                                       / (2 * m_RootK);
        _concentrations2.tail(nbPostInfusionPoints) += (A2PostInf * alphaPostInfLogV.tail(nbPostInfusionPoints)
                                                        + B2PostInf * betaPostInfLogV.tail(nbPostInfusionPoints))
                                                       / (2 * m_RootK);
    }
}

class TwoCompartmentInfusionMacro : public TwoCompartmentInfusionMicro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentInfusionMacro)
public:
    TwoCompartmentInfusionMacro();

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};


class TwoCompartmentInfusionMacroRatios : public TwoCompartmentInfusionMacro
{
    INTAKEINTERVALCALCULATOR_UTILS(TwoCompartmentInfusionMacroRatios)
public:
    TwoCompartmentInfusionMacroRatios() = default;

    /// \brief Returns the list of required PK parameters Ids
    /// \return The list of required PK parameters Ids
    static std::vector<std::string> getParametersId();

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TWOCOMPARTMENTINFUSION_H
