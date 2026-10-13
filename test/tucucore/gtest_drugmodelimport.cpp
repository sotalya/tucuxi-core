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

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"

#include "gtest_drugmodelimport_xmlstrings.h"

using namespace Tucuxi::Core;

TEST(Core_TestDrugModelImport, DrugModelImportFake)
{
    std::unique_ptr<DrugModel> fake;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importFromString(fake, DRUG1);
    //        DrugModelImport::Result result = importer->importFromFile(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/newdrugs/last/ch.heig-vd.ezechiel.example.xml");
    //        DrugModelImport::Result result = importer->importFromFile(imatinib, "/home/ythoma/docs/ezechiel/git/dev/src/drugs2/ch.tucuxi.busulfan_children.xml");

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(fake != nullptr);
}

TEST(Core_TestDrugModelImport, DrugModelImportBusulfan1)
{
    std::unique_ptr<DrugModel> busulfan;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importFromString(busulfan, BUSULFAN1);

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(busulfan != nullptr);
}

TEST(Core_TestDrugModelImport, IovRejectsMoreThanOneStdDev)
{
    // Inject an <iov> block right after the first <bsv> of a known-good model.
    // One IOV standard deviation must import, more than one must be rejected.
    std::size_t const pos = BUSULFAN1.find("</bsv>");
    ASSERT_NE(pos, std::string::npos);
    std::size_t const insertAt = pos + std::string("</bsv>").size();

    std::string const oneIov = "<iov><stdDevs><stdDev>0.1</stdDev></stdDevs></iov>";
    std::string const twoIov = "<iov><stdDevs><stdDev>0.1</stdDev><stdDev>0.2</stdDev></stdDevs></iov>";

    {
        std::string xml = BUSULFAN1;
        xml.insert(insertAt, oneIov);
        std::unique_ptr<DrugModel> model;
        auto importer = std::make_unique<DrugModelImport>();
        DrugModelImport::Status const status = importer->importFromString(model, xml);
        ASSERT_EQ(status, DrugModelImport::Status::Ok);
    }
    {
        std::string xml = BUSULFAN1;
        xml.insert(insertAt, twoIov);
        std::unique_ptr<DrugModel> model;
        auto importer = std::make_unique<DrugModelImport>();
        DrugModelImport::Status const status = importer->importFromString(model, xml);
        ASSERT_EQ(status, DrugModelImport::Status::Error);
    }
}

TEST(Core_TestDrugModelImport, IovInheritsBsvLaw)
{
    // Inject one IOV after the first BSV of a known-good model, whose first
    // BSV is proportional. The imported IOV must inherit the BSV distribution
    // law, not default to None.
    std::size_t const pos = BUSULFAN1.find("</bsv>");
    ASSERT_NE(pos, std::string::npos);
    std::string xml = BUSULFAN1;
    xml.insert(pos + std::string("</bsv>").size(), "<iov><stdDevs><stdDev>0.1</stdDev></stdDevs></iov>");

    std::unique_ptr<DrugModel> model;
    auto importer = std::make_unique<DrugModelImport>();
    ASSERT_EQ(importer->importFromString(model, xml), DrugModelImport::Status::Ok);
    ASSERT_NE(model, nullptr);

    const AnalyteSet* analyteSet = model->getAnalyteSet("");
    ASSERT_NE(analyteSet, nullptr);
    const ParameterSetDefinition& params = analyteSet->getDispositionParameters();

    bool foundIov = false;
    for (size_t i = 0; i < params.getNbParameters(); ++i) {
        const ParameterDefinition* p = params.getParameter(i);
        ASSERT_NE(p, nullptr);
        if (p->hasIov()) {
            foundIov = true;
            // The IOV law equals the BSV law, and is not the default None.
            EXPECT_EQ(p->getIov().getType(), p->getVariability().getType());
            EXPECT_NE(p->getIov().getType(), ParameterVariabilityType::None);
        }
    }
    EXPECT_TRUE(foundIov);
}

TEST(Core_TestDrugModelImport, IovOnNoneTypedBsvIsInert)
{
    // A BSV of type None makes the parameter not variable, so it does nothing.
    // An IOV reuses the BSV law, so on a None-typed BSV the IOV reuses the None
    // law and is equally inert. Adding the IOV must not change the outcome.
    std::string const bsvProp = "<bsvType>proportional</bsvType>";
    std::size_t const typePos = BUSULFAN1.find(bsvProp);
    ASSERT_NE(typePos, std::string::npos);

    std::string noneBsv = BUSULFAN1;
    noneBsv.replace(typePos, bsvProp.size(), "<bsvType>none</bsvType>");

    {
        // None-typed BSV, no IOV, a valid model.
        std::unique_ptr<DrugModel> model;
        auto importer = std::make_unique<DrugModelImport>();
        ASSERT_EQ(importer->importFromString(model, noneBsv), DrugModelImport::Status::Ok);
    }
    {
        // None-typed BSV plus an IOV, still valid, the IOV reuses the None law.
        std::string xml = noneBsv;
        std::size_t const bsvEnd = xml.find("</bsv>");
        ASSERT_NE(bsvEnd, std::string::npos);
        xml.insert(bsvEnd + std::string("</bsv>").size(), "<iov><stdDevs><stdDev>0.1</stdDev></stdDevs></iov>");
        std::unique_ptr<DrugModel> model;
        auto importer = std::make_unique<DrugModelImport>();
        ASSERT_EQ(importer->importFromString(model, xml), DrugModelImport::Status::Ok);
    }
}

TEST(Core_TestDrugModelImport, DrugModelImportOperation)
{
    std::unique_ptr<Operation> operation;
    auto importer = std::make_unique<DrugModelImport>();

    DrugModelImport::Status status = importer->importOperationFromString(operation, OPERATION_XML);

    ASSERT_EQ(status, DrugModelImport::Status::Ok);

    ASSERT_TRUE(operation != nullptr);
}
