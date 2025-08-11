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


#include <memory>

#include <date/date.h>
#include <gtest/gtest.h>

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"

#include "tucuquery/querycomputer.h"

#include "../tucucore/drugmodels/buildmod202.h"

using namespace std::chrono_literals;
using namespace date;

using namespace Tucuxi::Core;
using namespace Tucuxi::Query;

///
/// \brief Test with a 60-minute infusion
///
TEST(Core_TestMod202, rk4_infu60)
{
    DrugModelImport importer;

    BuildMod202 builder;
    std::unique_ptr<DrugModel> drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);


    // Getting the drug model

    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();

    auto drugModelRepository =
            dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);


    dynamic_cast<DrugModelRepository*>(drugModelRepository)->addDrugModel(drugModel.get());

    QueryComputer computer;
    ComputingQueryResponse response;
    computer.compute(tqf60, response);

    auto data0 = response.getRequestResponses().at(0).m_computingResponse->getData();
    auto concentrations0 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data0);

    auto data1 = response.getRequestResponses().at(1).m_computingResponse->getData();
    auto concentrations1 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data1);

    for (size_t i = 0; i < 20; i++) {
        EXPECT_FLOAT_EQ(
                concentrations0->getData()[0].m_concentrations[0][i * 10],
                concentrations1->getData()[0].m_concentrations[0][i]);
    }

    auto data2 = response.getRequestResponses().at(2).m_computingResponse->getData();
    auto concentrations2 = dynamic_cast<const Tucuxi::Core::SinglePointsData*>(data2);

    EXPECT_FLOAT_EQ(concentrations2->m_concentrations[0][1], concentrations0->getData()[0].m_concentrations[0][117]);

    if (component != nullptr) {
        delete component;
    }
}

///
/// \brief Test with a 65-minute infusion
///
TEST(Core_TestMod202, rk4_infu65)
{
    DrugModelImport importer;

    BuildMod202 builder;
    std::unique_ptr<DrugModel> drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);


    // Getting the drug model

    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();

    auto drugModelRepository =
            dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);


    dynamic_cast<DrugModelRepository*>(drugModelRepository)->addDrugModel(drugModel.get());

    QueryComputer computer;
    ComputingQueryResponse response;
    computer.compute(tqf65, response);

    auto data0 = response.getRequestResponses().at(0).m_computingResponse->getData();
    auto concentrations0 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data0);

    auto data1 = response.getRequestResponses().at(1).m_computingResponse->getData();
    auto concentrations1 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data1);

    for (size_t i = 0; i < 20; i++) {
        EXPECT_FLOAT_EQ(concentrations0->getData()[0].m_times[0][i * 10], concentrations1->getData()[0].m_times[0][i]);

        EXPECT_FLOAT_EQ(
                concentrations0->getData()[0].m_concentrations[0][i * 10],
                concentrations1->getData()[0].m_concentrations[0][i]);
    }

    auto data2 = response.getRequestResponses().at(2).m_computingResponse->getData();
    auto concentrations2 = dynamic_cast<const Tucuxi::Core::SinglePointsData*>(data2);

    EXPECT_FLOAT_EQ(concentrations2->m_concentrations[0][1], concentrations0->getData()[0].m_concentrations[0][117]);

    if (component != nullptr) {
        delete component;
    }
}

///
/// \brief Test with a 52-minute infusion
///
TEST(Core_TestMod202, rk4_infu52)
{
    DrugModelImport importer;

    BuildMod202 builder;
    std::unique_ptr<DrugModel> drugModel = builder.buildDrugModel();

    ASSERT_TRUE(drugModel != nullptr);

    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ASSERT_TRUE(component != nullptr);


    // Getting the drug model

    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();

    auto drugModelRepository =
            dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);


    dynamic_cast<DrugModelRepository*>(drugModelRepository)->addDrugModel(drugModel.get());

    QueryComputer computer;
    ComputingQueryResponse response;
    computer.compute(tqf52, response);

    auto data0 = response.getRequestResponses().at(0).m_computingResponse->getData();
    auto concentrations0 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data0);

    auto data1 = response.getRequestResponses().at(1).m_computingResponse->getData();
    auto concentrations1 = dynamic_cast<const Tucuxi::Core::ConcentrationData*>(data1);

    for (size_t i = 0; i < 20; i++) {
        EXPECT_FLOAT_EQ(concentrations0->getData()[0].m_times[0][i * 10], concentrations1->getData()[0].m_times[0][i]);

        EXPECT_FLOAT_EQ(
                concentrations0->getData()[0].m_concentrations[0][i * 10],
                concentrations1->getData()[0].m_concentrations[0][i]);
    }

    auto data2 = response.getRequestResponses().at(2).m_computingResponse->getData();
    auto concentrations2 = dynamic_cast<const Tucuxi::Core::SinglePointsData*>(data2);

    EXPECT_FLOAT_EQ(concentrations2->m_concentrations[0][1], concentrations0->getData()[0].m_concentrations[0][117]);

    if (component != nullptr) {
        delete component;
    }
}
