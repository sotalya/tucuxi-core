//@@lisence@@

#include <cstdio>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>

#include "drugfilevalidator.h"

#include "tucucommon/loggerhelper.h"

#include "tucucore/covariateextractor.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodelchecker.h"
#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/parametersextractor.h"
#include "tucucore/pkmodel.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

//using namespace rapidjson;
using namespace std;


namespace Tucuxi {
namespace Core {

DrugFileValidator::DrugFileValidator() = default;


class ExpectedParameters
{
public:
    std::string m_id;
    Tucuxi::Core::Value m_value;
};


bool DrugFileValidator::validate(const std::string& _drugFileName, const std::string& _testFileName)
{

    static const string DATA_FORMAT = "%Y-%m-%dT%H:%M:%S";


    Tucuxi::Common::LoggerHelper logger;
    rapidjson::Document document; // Default template parameter uses UTF8 and MemoryPoolAllocator.

    std::ifstream t(_testFileName);
    std::stringstream buffer;
    buffer << t.rdbuf();

    if (document.Parse(buffer.str().c_str()).HasParseError()) {
        logger.error("Can not import the tests file");
        return false;
    }

    Tucuxi::Core::PatientVariates covariates;

    std::unique_ptr<Tucuxi::Core::DrugModel> drugModel;

    DrugModelImport importer;
    if (importer.importFromFile(drugModel, _drugFileName) != DrugModelImport::Status::Ok) {
        logger.error("Can not import the drug file. {}", importer.getErrorMessage());
        return false;
    }



    DrugModelChecker checker;

    PkModelCollection pkCollection;

    if (!defaultPopulate(pkCollection)) {
        logger.error("Could not populate the Pk models collection. No model will be available");
        return false;
    }

    DrugModelChecker::CheckerResult_t checkerResult = checker.checkDrugModel(drugModel.get(), &pkCollection);
    if (!checkerResult.m_ok) {
        logger.error(checkerResult.m_errorMessage);
        return false;
    }

    bool validationResult = true;
    {
        const rapidjson::Value& parametersTests =
                document["parameterstests"]; // Using a reference for consecutive access is handy and faster.
        if (!parametersTests.IsArray()) {
            logger.error("The parameterstests shall be an array, but it is not");
            return false;
        }
        for (rapidjson::SizeType i = 0; i < parametersTests.Size(); i++) { // rapidjson uses SizeType instead of size_t.
            const rapidjson::Value& test = parametersTests[i];
            const rapidjson::Value& covariates = test["covariates"];
            const rapidjson::Value& parameters = test["parameters"];

            std::string testId = test["testId"].GetString();
            std::string comment = test["comment"].GetString();
            std::string sampleDateString;
            Common::DateTime sampleDate;

            if (test.HasMember("sampleDate")) {
                sampleDateString = test["sampleDate"].GetString();
                sampleDate = Common::DateTime(sampleDateString, DATA_FORMAT);
            }
            else {
                sampleDate =
                        Common::DateTime(Common::DateTime::fromDurationSinceEpoch(std::chrono::hours(365 * 2 * 24)));
            }

            Tucuxi::Common::DateTime startDate(sampleDate);
            startDate.addYears(-1);
            Tucuxi::Common::DateTime endDate(sampleDate);
            startDate.addYears(1);

            Tucuxi::Core::PatientVariates patientVariates;

            for (rapidjson::Value::ConstValueIterator itr = covariates.Begin(); itr != covariates.End(); ++itr) {
                //printf("Covariate \"%s\" = %\n", (*itr)["id"].GetString(), (*itr)["value"].GetDouble());
                std::string id = (*itr)["id"].GetString();
                std::string value = (*itr)["value"].GetString();
                std::string unitString = (*itr)["unit"].GetString();
                std::string dataTypeString = (*itr)["dataType"].GetString();
                Tucuxi::Core::DataType dataType;

                if (dataTypeString == "int") {
                    dataType = Core::DataType::Int;
                }
                else if (dataTypeString == "double") {
                    dataType = Core::DataType::Double;
                }
                else if (dataTypeString == "bool") {
                    dataType = Core::DataType::Bool;
                }
                else if (dataTypeString == "date") {
                    dataType = Core::DataType::Date;
                }
                else {
                    // Maybe we could have an Undefined here
                    dataType = Core::DataType::Int;
                    logger.error("Data type unknown");
                    validationResult = false;
                }

                // If the covariate is a date, go through a DateTime to reconvert it to a string
                // to be sure it has the correct format
                if (dataType == Core::DataType::Date) {

                    Common::DateTime valueAsDate(value, DATA_FORMAT);

                    value = Tucuxi::Common::Utils::varToString(valueAsDate);
                }



                TucuUnit unit(unitString);
                Tucuxi::Common::DateTime date(Common::DateTime::fromDurationSinceEpoch(std::chrono::hours(1)));
                std::unique_ptr<Tucuxi::Core::PatientCovariate> covariate =
                        std::make_unique<Tucuxi::Core::PatientCovariate>(id, value, dataType, unit, date);
                patientVariates.push_back(std::move(covariate));
            }


            CovariateSeries covariatesSeries;
            {
                CovariateExtractor covariateExtractor(drugModel->getCovariates(), patientVariates, startDate, endDate);
                ComputingStatus covariateExtractionResult = covariateExtractor.extract(covariatesSeries);

                if (covariateExtractionResult != ComputingStatus::Ok) {
                    logger.error("Can not extract covariates");
                    return false;
                }
            }

            // TODO : This should not necessarily be the default formulation and route
            // Should get rid of the next 3 lines
            const AnalyteGroupId analyteGroupId = drugModel->getAnalyteSet()->getId();
            const Formulation formulation =
                    drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
            const AdministrationRoute route = drugModel->getFormulationAndRoutes()
                                                      .getDefault()
                                                      ->getFormulationAndRoute()
                                                      .getAdministrationRoute();


            ParameterDefinitionIterator it = drugModel->getParameterDefinitions(analyteGroupId, formulation, route);

            ParametersExtractor parameterExtractor(covariatesSeries, it, startDate, endDate);

            ComputingStatus parametersExtractionResult;


            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);


            if (parametersExtractionResult != ComputingStatus::Ok) {
                logger.error("Can not extract parameters");
                return false;
            }

            ParameterSetSeries parameterSeries;

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, parameterSeries);
            if (parametersExtractionResult != ComputingStatus::Ok) {
                logger.error("Can not consolidate parameters");
                return false;
            }

            // And for each event, update the parameters that change at that time

            const double threshold = 0.0001;

            ParameterSetEventPtr calculatedParameters = parameterSeries.getAtTime(sampleDate);

            for (rapidjson::Value::ConstValueIterator itr = parameters.Begin(); itr != parameters.End(); ++itr) {
                bool found = false;


                auto pit = calculatedParameters->begin();
                while (pit != calculatedParameters->end()) {
                    if ((*itr)["id"].GetString() == pit->getParameterId()) {
                        found = true;
                        if (std::abs((*itr)["value"].GetDouble() - pit->getValue()) > threshold) {
                            logger.error(
                                    "Test {}: Error with calculation of parameter {}. Expected {}, calculated {}",
                                    testId,
                                    pit->getParameterId(),
                                    (*itr)["value"].GetDouble(),
                                    pit->getValue());
                            validationResult = false;
                        }
                    }
                    pit++;
                }
                if (!found) {
                    logger.error("Could not find parameter {}", (*itr)["id"].GetString());
                    validationResult = false;
                }
            }
        }
    }

    if (validationResult) {
        logger.info("The drug file passed the validation tests");
    }
    return validationResult;
}

} // namespace Core
} // namespace Tucuxi
