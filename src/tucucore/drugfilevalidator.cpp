#include "drugfilevalidator.h"

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <cstdio>
#include <iostream>
#include <iosfwd>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>

#include "tucucommon/loggerhelper.h"

#include "tucucore/drugmodelimport.h"
#include "tucucore/drugtreatment/patientcovariate.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/parametersextractor.h"
#include "tucucore/drugmodel/drugmodel.h"

//using namespace rapidjson;
using namespace std;


namespace Tucuxi {
namespace Core {

DrugFileValidator::DrugFileValidator()
{

}


class ExpectedParameters
{
public:
    std::string m_id;
    Tucuxi::Core::Value m_value;
};


bool DrugFileValidator::validate(std::string drugFileName, std::string testFileName)
{

    const string m_sDATE_FORMAT = "%Y-%m-%dT%H:%M:%S";


    Tucuxi::Common::LoggerHelper logger;
    rapidjson::Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

    std::ifstream t(testFileName);
    std::stringstream buffer;
    buffer << t.rdbuf();

    if (document.Parse(buffer.str().c_str()).HasParseError()) {
        logger.error("Can not import the tests file");
        return false;
    }

    Tucuxi::Core::PatientVariates covariates;

    Tucuxi::Core::DrugModel *dModel;

    DrugModelImport importer;
    if (importer.importFromFile(dModel, drugFileName) != DrugModelImport::Result::Ok) {
        logger.error("Can not import the drug file");
        return false;
    }

    std::unique_ptr<Tucuxi::Core::DrugModel> drugModel(dModel);

    bool validationResult = true;
    {
        const rapidjson::Value& parametersTests = document["parameterstests"]; // Using a reference for consecutive access is handy and faster.
        assert(parametersTests.IsArray());
        for (rapidjson::SizeType i = 0; i < parametersTests.Size(); i++) {// rapidjson uses SizeType instead of size_t.
            const rapidjson::Value &test = parametersTests[i];
            const rapidjson::Value &covariates = test["covariates"];
            const rapidjson::Value &parameters = test["parameters"];

            std::string testId = test["testId"].GetString();
            std::string comment = test["comment"].GetString();
            std::string sampleDateString;
            Common::DateTime sampleDate;

            if (test.HasMember("sampleDate")) {
                sampleDateString = test["sampleDate"].GetString();
                sampleDate = Common::DateTime(sampleDateString, m_sDATE_FORMAT);
            }
            else {
                sampleDate = Common::DateTime(std::chrono::hours(365 * 2 * 24));
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
                } else if (dataTypeString == "double") {
                    dataType = Core::DataType::Double;
                } else if (dataTypeString == "bool") {
                    dataType = Core::DataType::Bool;
                } else if (dataTypeString == "date") {
                    dataType = Core::DataType::Date;
                } else {
                    logger.error("Data type unknown");
                    validationResult = false;
                }

                // If the covariate is a date, go through a DateTime to reconvert it to a string
                // to be sure it has the correct format
                if (dataType == Core::DataType::Date) {

                    Common::DateTime valueAsDate(value, m_sDATE_FORMAT);

                    value = Tucuxi::Common::Utils::varToString(valueAsDate);
                }



                Unit unit(unitString);
                Tucuxi::Common::DateTime date(std::chrono::hours(1));
                Tucuxi::Core::PatientCovariate *covariate = new Tucuxi::Core::PatientCovariate(id, value, dataType, unit, date);
                patientVariates.push_back(std::unique_ptr<Tucuxi::Core::PatientCovariate>(covariate));
            }


            CovariateSeries covariatesSeries;
            {
                CovariateExtractor covariateExtractor(drugModel->getCovariates(),
                                                      patientVariates,
                                                      startDate,
                                                      endDate);
                CovariateExtractor::Result covariateExtractionResult = covariateExtractor.extract(covariatesSeries);

                if (covariateExtractionResult != CovariateExtractor::Result::Ok) {
                    logger.error("Can not extract covariates");
                    return false;
                }
            }

            // TODO : This should not necessarily be the default formulation and route
            // Should get rid of the next 3 lines
            const std::string analyteId = drugModel->getAnalyteSet()->getId();
            const Formulation formulation = drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getFormulation();
            const AdministrationRoute route = drugModel->getFormulationAndRoutes().getDefault()->getFormulationAndRoute().getAdministrationRoute();


            ParameterDefinitionIterator it = drugModel->getParameterDefinitions(analyteId, formulation, route);

            ParametersExtractor parameterExtractor(covariatesSeries,
                                                   it,
                                                   startDate,
                                                   endDate);

            ParametersExtractor::Result parametersExtractionResult;


            ParameterSetSeries intermediateParameterSeries;

            parametersExtractionResult = parameterExtractor.extract(intermediateParameterSeries);


            if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
                logger.error("Can not extract parameters");
                return false;
            }

            ParameterSetSeries parameterSeries;

            // The intermediateParameterSeries contains changes of parameters, so we build a full set of parameter
            // for each event.
            parametersExtractionResult = parameterExtractor.buildFullSet(intermediateParameterSeries, parameterSeries);
            if (parametersExtractionResult != ParametersExtractor::Result::Ok) {
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
                            logger.error("Error with calculation of parameter {}. Expected {}, calculated {}", pit->getParameterId(), (*itr)["value"].GetDouble(), pit->getValue());
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
