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


#include <gtest/gtest.h>

#include "tucucommon/timeofday.h"
#include "tucucommon/utils.h"

#include "tucucore/drugdomainconstraintsevaluator.h"
#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/treatmentdrugmodelcompatibilitychecker.h"

#include "testutils.h"
#include "gtest_core.h"
#include "drugmodels/buildimatinib.h"

using namespace Tucuxi::Core;
using namespace Tucuxi::Common::Utils;

TEST(Core_TestTreatmentDrugModelCompatibilityChecker, BadInputs)
{
    std::unique_ptr<DrugTreatment> treatment;
    std::unique_ptr<DrugModel> drugModel;
    TreatmentDrugModelCompatibilityChecker checker;
    drugModel = std::make_unique<DrugModel>();
    ASSERT_FALSE(checker.checkCompatibility(treatment.get(), drugModel.get()));

    drugModel.reset();


    FormulationAndRoute formulationAndRoute(Formulation::ParenteralSolution, AdministrationRoute::Nasal, "something");
    treatment = buildDrugTreatment(formulationAndRoute, DateTime::now());
    ASSERT_FALSE(checker.checkCompatibility(treatment.get(), drugModel.get()));

    BuildImatinib builder;
    drugModel = builder.buildDrugModel();

    ASSERT_FALSE(checker.checkCompatibility(treatment.get(), drugModel.get()));

}
