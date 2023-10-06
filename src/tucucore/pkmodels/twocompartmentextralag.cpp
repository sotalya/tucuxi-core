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


#include <Eigen/Dense>

#include "tucucore/pkmodels/twocompartmentextralag.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

TwoCompartmentExtraLagMicro::TwoCompartmentExtraLagMicro()
    : IntakeIntervalCalculatorBase<3, TwoCompartmentExtraLagExponentials>(
            std::make_unique<PertinentTimesCalculatorStandard>())
{
}

std::vector<std::string> TwoCompartmentExtraLagMicro::getParametersId()
{
    return {"Ke", "V1", "K12", "K21", "Ka", "F", "Tlag"};
}

bool TwoCompartmentExtraLagMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 7, "The number of parameters should be equal to 7.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);

    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(m_Ke, "Ke");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkStrictlyPositiveValue(m_K12, "K12");
    bOK &= checkStrictlyPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    if (m_nbPoints == 2) {
        m_nbPoints0 = static_cast<Eigen::Index>(std::min(
                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        //    m_nbPoints0 = std::min(m_nbPoints, std::max(2, static_cast<int>((m_Tlag / m_Int) * static_cast<double>(m_nbPoints))));
        m_nbPoints0 = std::min(
                static_cast<Eigen::Index>(m_nbPoints),
                std::max(
                        Eigen::Index{2},
                        static_cast<Eigen::Index>(std::min(
                                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)),
                                ceil(static_cast<double>(m_nbPoints))))));
    }
    m_nbPoints1 = m_nbPoints - m_nbPoints0;


    return bOK;
}


inline bool TwoCompartmentExtraLagMicro::compute(
        const Residuals& _inResiduals,
        Eigen::VectorXd& _concentrations1,
        Eigen::VectorXd& _concentrations2,
        Eigen::VectorXd& _concentrations3)
{
    Value A = NAN;       // NOLINT(readability-identifier-naming)
    Value B = NAN;       // NOLINT(readability-identifier-naming)
    Value C = NAN;       // NOLINT(readability-identifier-naming)
    Value divider = NAN; // NOLINT(readability-identifier-naming)

    if (m_Tlag <= 0.0) {

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
                         - m_Ka
                                   * (m_Ka * sumResid13 * (2 * m_Ke + m_RootK)
                                      - m_Ke * resid1 * (3 * m_Ke + 2 * m_RootK)));

        B = std::pow(m_K12, 3) * m_Ka * resid1
            + diffK21Ka
                      * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13)
                         + ((m_Ka - m_Ke) * (-m_Ke * resid1 + m_K21 * (resid1 + 2 * resid2))
                            + m_Ka * diffK21Ke * resid3)
                                   * m_RootK)
            + std::pow(m_K12, 2)
                      * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
                         - m_Ka * (m_Ka * sumResid13 + resid1 * (-3 * m_Ke + m_RootK)))
            + m_K12
                      * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3)
                         + m_Ka * (m_Ke * resid1 * (3 * m_Ke - 2 * m_RootK) - m_Ka * sumResid13 * (2 * m_Ke - m_RootK))
                         + m_K21
                                   * (-2 * std::pow(m_Ka, 2) * sumResid13 + 2 * m_Ka * m_Ke * sumResid13
                                      + m_Ka * (2 * resid2 + resid3) * m_RootK
                                      + m_Ke * resid1 * (-2 * m_Ke + m_RootK)));

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

        // I added these lines to get rid of calculus approximations making the first concentration to be potentially negative.
        if (abs(_concentrations1[0]) < 1e-10) {
            _concentrations1[0] = 0.0;
        }

        return true;
    }
    else {

        Concentration resid1 = _inResiduals[0];
        Concentration resid2 = _inResiduals[1];
        Concentration resid3 = _inResiduals[2];
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
                         - m_Ka
                                   * (m_Ka * sumResid13 * (2 * m_Ke + m_RootK)
                                      - m_Ke * resid1 * (3 * m_Ke + 2 * m_RootK)));

        B = std::pow(m_K12, 3) * m_Ka * resid1
            + diffK21Ka
                      * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13)
                         + ((m_Ka - m_Ke) * (-m_Ke * resid1 + m_K21 * (resid1 + 2 * resid2))
                            + m_Ka * diffK21Ke * resid3)
                                   * m_RootK)
            + std::pow(m_K12, 2)
                      * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
                         - m_Ka * (m_Ka * sumResid13 + resid1 * (-3 * m_Ke + m_RootK)))
            + m_K12
                      * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3)
                         + m_Ka * (m_Ke * resid1 * (3 * m_Ke - 2 * m_RootK) - m_Ka * sumResid13 * (2 * m_Ke - m_RootK))
                         + m_K21
                                   * (-2 * std::pow(m_Ka, 2) * sumResid13 + 2 * m_Ka * m_Ke * sumResid13
                                      + m_Ka * (2 * resid2 + resid3) * m_RootK
                                      + m_Ke * resid1 * (-2 * m_Ke + m_RootK)));

        C = -2 * diffK21Ka * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) * resid3;

        divider = std::pow((sumK12K21 - 2 * m_Ka + m_Ke) * m_RootK, 2)
                  - std::pow(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2);

        if (!checkCondition(divider != 0.0, "Dividing by zero.")) {
            return false;
        }

        // Calculate concentrations of compartment 1
        Eigen::VectorXd concentrations1 =
                -2
                * (B * exponentials(Exponentials::Beta) + A * exponentials(Exponentials::Alpha)
                   + C * exponentials(Exponentials::Ka))
                / divider;

        Concentration resid1post =
                -2 * (B * exp(-m_Beta * m_Tlag) + A * exp(-m_Alpha * m_Tlag) + C * exp(-m_Ka * m_Tlag)) / divider;

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
        Eigen::VectorXd concentrations2 =
                2
                * (B * exponentials(Exponentials::Beta) + A * exponentials(Exponentials::Alpha)
                   + C * exponentials(Exponentials::Ka))
                / divider;
        Eigen::VectorXd concentrations3 = exponentials(Exponentials::Ka) * resid3;

        // Let's now compute the part after Tlag

        // First the new residuals
        resid1 = resid1post;
        resid2 = 2 * (B * exp(-m_Beta * m_Tlag) + A * exp(-m_Alpha * m_Tlag) + C * exp(-m_Ka * m_Tlag)) / divider;
        resid3 = exp(-m_Ka * m_Tlag) * resid3 + m_F * m_D / m_V1;
        sumResid13 = resid1 + resid3;
        sumK12K21 = m_K12 + m_K21;
        sumK21Ke = m_K21 + m_Ke;
        diffK21Ka = m_K21 - m_Ka;
        diffK21Ke = m_K21 - m_Ke;
        powDiffK21Ke = std::pow(diffK21Ke, 2);


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
                         - m_Ka
                                   * (m_Ka * sumResid13 * (2 * m_Ke + m_RootK)
                                      - m_Ke * resid1 * (3 * m_Ke + 2 * m_RootK)));

        B = std::pow(m_K12, 3) * m_Ka * resid1
            + diffK21Ka
                      * (powDiffK21Ke * (-m_Ke * resid1 + m_Ka * sumResid13)
                         + ((m_Ka - m_Ke) * (-m_Ke * resid1 + m_K21 * (resid1 + 2 * resid2))
                            + m_Ka * diffK21Ke * resid3)
                                   * m_RootK)
            + std::pow(m_K12, 2)
                      * (m_K21 * (-m_Ke * resid1 + m_Ka * (3 * resid1 + resid3))
                         - m_Ka * (m_Ka * sumResid13 + resid1 * (-3 * m_Ke + m_RootK)))
            + m_K12
                      * (std::pow(m_K21, 2) * (3 * m_Ka * resid1 - 2 * m_Ke * resid1 + 2 * m_Ka * resid3)
                         + m_Ka * (m_Ke * resid1 * (3 * m_Ke - 2 * m_RootK) - m_Ka * sumResid13 * (2 * m_Ke - m_RootK))
                         + m_K21
                                   * (-2 * std::pow(m_Ka, 2) * sumResid13 + 2 * m_Ka * m_Ke * sumResid13
                                      + m_Ka * (2 * resid2 + resid3) * m_RootK
                                      + m_Ke * resid1 * (-2 * m_Ke + m_RootK)));

        C = -2 * diffK21Ka * m_Ka * (std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke) * resid3;

        divider = std::pow((sumK12K21 - 2 * m_Ka + m_Ke) * m_RootK, 2)
                  - std::pow(std::pow(m_K12, 2) + powDiffK21Ke + 2 * m_K12 * sumK21Ke, 2);

        if (!checkCondition(divider != 0.0, "Dividing by zero.")) {
            return false;
        }

        // Calculate concentrations of compartment 1
        Eigen::VectorXd concentrations1post =
                -2
                * (B * exponentials(Exponentials::BetaPost) + A * exponentials(Exponentials::AlphaPost)
                   + C * exponentials(Exponentials::KaPost))
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
        Eigen::VectorXd concentrations2post =
                2
                * (B * exponentials(Exponentials::BetaPost) + A * exponentials(Exponentials::AlphaPost)
                   + C * exponentials(Exponentials::KaPost))
                / divider;
        Eigen::VectorXd concentrations3post = exponentials(Exponentials::KaPost) * resid3;

        _concentrations1 = Eigen::VectorXd(m_nbPoints);
        _concentrations2 = Eigen::VectorXd(m_nbPoints);
        _concentrations3 = Eigen::VectorXd(m_nbPoints);
        for (Eigen::Index i = 0; i < static_cast<Eigen::Index>(m_nbPoints0); i++) {
            _concentrations1[i] = concentrations1[i];
            _concentrations2[i] = concentrations2[i];
            _concentrations3[i] = concentrations3[i];
        }
        for (Eigen::Index i = static_cast<Eigen::Index>(m_nbPoints0); i < static_cast<Eigen::Index>(m_nbPoints); i++) {
            _concentrations1[i] = concentrations1post[i];
            _concentrations2[i] = concentrations2post[i];
            _concentrations3[i] = concentrations3post[i];
        }

        // I added these lines to get rid of calculus approximations making the first concentration to be potentially negative.
        if (abs(_concentrations1[0]) < 1e-10) {
            _concentrations1[0] = 0.0;
        }

        return true;
    }
}

void TwoCompartmentExtraLagMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::Ka, (-m_Ka * _times).array().exp());
    Eigen::VectorXd postTimes = _times - m_Tlag * Eigen::VectorXd::Ones(_times.size());
    setExponentials(Exponentials::AlphaPost, (-m_Alpha * postTimes).array().exp());
    setExponentials(Exponentials::BetaPost, (-m_Beta * postTimes).array().exp());
    setExponentials(Exponentials::KaPost, (-m_Ka * postTimes).array().exp());
}


bool TwoCompartmentExtraLagMicro::computeConcentrations(
        const Residuals& _inResiduals, bool _isAll, MultiCompConcentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2;
    Eigen::VectorXd concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);

    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    if (!bOK) {
        // If computing went wrong, then stop here
        return bOK;
    }
    // Return residuals of comp1, comp2 and comp3
    _outResiduals[firstCompartment] = concentrations1[m_nbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_nbPoints - 1];
    _outResiduals[thirdCompartment] = concentrations3[m_nbPoints - 1];

    // Return concentrations of comp1, comp2 and comp3
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    bOK &= checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

bool TwoCompartmentExtraLagMicro::computeConcentration(
        const Value& _atTime,
        const Residuals& _inResiduals,
        bool _isAll,
        MultiCompConcentrations& _concentrations,
        Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1;
    Eigen::VectorXd concentrations2, concentrations3;
    size_t firstCompartment = static_cast<size_t>(Compartments::First);
    size_t secondCompartment = static_cast<size_t>(Compartments::Second);
    size_t thirdCompartment = static_cast<size_t>(Compartments::Third);
    Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
    Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);


    // We fix nb points to be 2, to be coherent with calculateExponentials
    m_nbPoints = 2;
    // Then, depending on the fact that the time of interest is before the lag time
    // or not we set the number of points for each of these two intervals
    if (_atTime <= m_Tlag) {
        m_nbPoints0 = 1;
        m_nbPoints1 = 1;
    }
    else {
        m_nbPoints0 = 0;
        m_nbPoints1 = 2;
    }
    // Compute concentrations
    bool bOK = compute(_inResiduals, concentrations1, concentrations2, concentrations3);

    // return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    // TODO: add calcuation concentrations of second and third compartment and condtions
    TMP_UNUSED_PARAMETER(_isAll);

    // interval=0 means that it is the last cycle, so final residual = 0
    if (m_Int == 0) {
        concentrations1[atEndInterval] = 0;
        concentrations2[atEndInterval] = 0;
        concentrations3[atEndInterval] = 0;
    }

    // Return final residual of comp1, comp2 and comp3 (computation with m_Int (interval))
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2[atEndInterval];
    _outResiduals[thirdCompartment] = concentrations3[atEndInterval];

    bOK &= checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[secondCompartment] >= 0, "The concentration is negative.");
    bOK &= checkCondition(_outResiduals[thirdCompartment] >= 0, "The concentration is negative.");

    return bOK;
}

TwoCompartmentExtraLagMacro::TwoCompartmentExtraLagMacro() : TwoCompartmentExtraLagMicro() {}

std::vector<std::string> TwoCompartmentExtraLagMacro::getParametersId()
{
    return {"CL", "V1", "Q", "V2", "Ka", "F", "Tlag"};
}

bool TwoCompartmentExtraLagMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 7, "The number of parameters should be equal to 7.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    Value q = _parameters.getValue(ParameterId::Q);
    Value v2 = _parameters.getValue(ParameterId::V2);
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(q, "Q");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    if (m_nbPoints == 2) {
        m_nbPoints0 = static_cast<Eigen::Index>(std::min(
                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        //    m_nbPoints0 = std::min(m_nbPoints, std::max(2, static_cast<int>((m_Tlag / m_Int) * static_cast<double>(m_nbPoints))));
        m_nbPoints0 = std::min(
                static_cast<Eigen::Index>(m_nbPoints),
                std::max(
                        Eigen::Index{2},
                        static_cast<Eigen::Index>(std::min(
                                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)),
                                ceil(static_cast<double>(m_nbPoints))))));
    }
    m_nbPoints1 = m_nbPoints - m_nbPoints0;

    return bOK;
}


std::vector<std::string> TwoCompartmentExtraLagMacroRatios::getParametersId()
{
    return {"CL", "V1", "V2", "Ka", "F", "Tlag"};
}

bool TwoCompartmentExtraLagMacroRatios::checkInputs(
        const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    Value cl = _parameters.getValue(ParameterId::CL);
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value rQCL = _parameters.getValue(ParameterId::RQCL);
    Value rV2V1 = _parameters.getValue(ParameterId::RV2V1);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_F = _parameters.getValue(ParameterId::F);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    Value v2 = m_V1 * rV2V1;
    Value q = cl * rQCL;
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
    Value sumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((sumK * sumK) - (4 * m_K21 * m_Ke));
    m_Alpha = (sumK + m_RootK) / 2;
    m_Beta = (sumK - m_RootK) / 2;
    m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
    m_Int = (_intakeEvent.getInterval()).toHours();

    if (m_Tlag < 0.0) {
        m_Tlag = 0.0;
    }

    // check the inputs
    bool bOK = checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_Ka, "Ka");
    bOK &= checkStrictlyPositiveValue(m_F, "F");
    bOK &= checkStrictlyPositiveValue(cl, "The clearance");
    bOK &= checkStrictlyPositiveValue(m_V1, "V1");
    bOK &= checkStrictlyPositiveValue(v2, "V2");
    bOK &= checkPositiveValue(m_Alpha, "Alpha");
    bOK &= checkPositiveValue(m_Beta, "Beta");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    if (m_nbPoints == 2) {
        m_nbPoints0 = static_cast<Eigen::Index>(std::min(
                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)), ceil(static_cast<double>(m_nbPoints))));
    }
    else {
        //    m_nbPoints0 = std::min(m_nbPoints, std::max(2, static_cast<int>((m_Tlag / m_Int) * static_cast<double>(m_nbPoints))));
        m_nbPoints0 = std::min(
                static_cast<Eigen::Index>(m_nbPoints),
                std::max(
                        Eigen::Index{2},
                        static_cast<Eigen::Index>(std::min(
                                ceil(m_Tlag / m_Int * static_cast<double>(m_nbPoints)),
                                ceil(static_cast<double>(m_nbPoints))))));
    }
    m_nbPoints1 = m_nbPoints - m_nbPoints0;

    return bOK;
}

} // namespace Core
} // namespace Tucuxi
