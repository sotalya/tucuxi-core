//@@license@@

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugmodelrepository.h"

#include "tucuquery/querycomputer.h"
#include "tucuquery/querylogger.h"

#include "gtest_queryinputstrings.h"
#include "gtest_query.h"

TEST (TucuTestQuery, Query1CompTests){
    Tucuxi::Common::ComponentManager* pCmpMgr = Tucuxi::Common::ComponentManager::getInstance();

    auto drugModelRepository =
            dynamic_cast<Tucuxi::Core::DrugModelRepository*>(Tucuxi::Core::DrugModelRepository::createComponent());

    pCmpMgr->registerComponent("DrugModelRepository", drugModelRepository);

    Tucuxi::Core::DrugModelImport importer;

    std::unique_ptr<Tucuxi::Core::DrugModel> drugModel;

    auto status = importer.importFromString(drugModel, tdd);
    EXPECT_EQ(status, Tucuxi::Core::DrugModelImport::Status::Ok);

    EXPECT_TRUE(drugModel != nullptr);

    drugModelRepository->addDrugModel(drugModel.get());

    std::string queryLogPath = "./log.txt";
    auto* queryLogger =
            dynamic_cast<Tucuxi::Query::QueryLogger*>(Tucuxi::Query::QueryLogger::createComponent(queryLogPath));

    pCmpMgr->registerComponent("QueryLogger", queryLogger);

    Tucuxi::Query::QueryComputer computer;
    Tucuxi::Query::ComputingQueryResponse response;
    computer.compute(tqf, response);

    EXPECT_EQ(response.getRequestResponses().size(), 2);

    auto& r1 = response.getRequestResponses()[0];
    auto& r2 = response.getRequestResponses()[1];

    auto resp1 = r1.m_computingResponse.get();
    auto resp2 = r2.m_computingResponse.get();

    const auto* d1 = dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(resp1->getData());
    const auto* d2 = dynamic_cast<const Tucuxi::Core::SinglePointsData*>(resp2->getData());

    //fructose_assert_eq(d2->m_concentrations[0].size(), 4);
    EXPECT_DOUBLE_EQ(d1->getData()[0].m_concentrations[0][20], d2->m_concentrations[0][0]);
}
