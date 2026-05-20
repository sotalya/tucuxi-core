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


#ifndef BUILDCONSTANTELIMINATION_H
#define BUILDCONSTANTELIMINATION_H


#include "tucucore/definitions.h"
#include "tucucore/drugmodel/drugmodel.h"

#include "drugmodelbuilder.h"

class BuildConstantElimination
{
public:
    BuildConstantElimination() {}


    // No targets are defined within the build, to let tests define various targets
    std::unique_ptr<Tucuxi::Core::DrugModel> buildDrugModel(
            Tucuxi::Core::ResidualErrorType _errorModelType = Tucuxi::Core::ResidualErrorType::NONE,
            std::vector<Tucuxi::Core::Value> _sigmas = {0.0},
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeA = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeM = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeR = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::ParameterVariabilityType _variabilityTypeS = Tucuxi::Core::ParameterVariabilityType::None,
            Tucuxi::Core::Value _variabilityValueA = 0.0,
            Tucuxi::Core::Value _variabilityValueM = 1.0,
            Tucuxi::Core::Value _variabilityValueR = 0.0,
            Tucuxi::Core::Value _variabilityValueS = 0.0);
};

#endif // BUILDCONSTANTELIMINATION_H
