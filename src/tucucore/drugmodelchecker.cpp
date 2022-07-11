//@@license@@

#include <chrono>

#include "drugmodelchecker.h"

#include "tucucommon/datetime.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/pkmodel.h"

namespace Tucuxi {
namespace Core {

DrugModelChecker::DrugModelChecker() = default;

DrugModelChecker::CheckerResult_t DrugModelChecker::checkDrugModel(
        const DrugModel* _drugModel, const PkModelCollection* _pkCollection)
{
    if (_drugModel == nullptr) {
        return {false, "The Error Model is nullptr"};
    }
    if (_pkCollection == nullptr) {
        return {false, "The Pk models collection is nullptr"};
    }

    // The invariants are basic checks implemented within the drug model classes
    if (!_drugModel->checkInvariants()) {
        return {false, "There is at least one drug model invariant that is not satisfied"};
    }

    // We need a PK model for each analyte group
    for (const auto& analyteGroup : _drugModel->getAnalyteSets()) {
        std::shared_ptr<PkModel> pkModel = _pkCollection->getPkModelFromId(analyteGroup->getPkModelId());
        if (pkModel == nullptr) {
            return {false,
                    "There is no PK model corresponding to the one defined in the drug model ("
                            + analyteGroup->getPkModelId() + ")"};
        }
    }


    // The analytes are used in various places in the drug model. Check coherency.
    DrugModelChecker::CheckerResult_t analyteResult = checkAnalytes(_drugModel);
    if (!analyteResult.m_ok) {
        return analyteResult;
    }

    // Checks that the parameters correspond to the PK model parameters
    DrugModelChecker::CheckerResult_t parametersResult = checkParameters(_drugModel, _pkCollection);
    if (!parametersResult.m_ok) {
        return parametersResult;
    }


    // Checks that the inputs of every formula corresponds to a covariate of the model
    DrugModelChecker::CheckerResult_t formulaInputsResult = checkFormulaInputs(_drugModel);
    if (!formulaInputsResult.m_ok) {
        return formulaInputsResult;
    }

    // Checks that every formula seems correct by randomizing inputs
    DrugModelChecker::CheckerResult_t operationsResult = checkOperations(_drugModel);
    if (!operationsResult.m_ok) {
        return operationsResult;
    }

    // Checks that the half life and the multiplier are correctly set
    DrugModelChecker::CheckerResult_t halfLifeResult = checkHalfLife(_drugModel);
    if (!halfLifeResult.m_ok) {
        return halfLifeResult;
    }

    return {true, ""};
}

bool contains(std::vector<std::string> _vector, const std::string& _s)
{
    return std::find(_vector.begin(), _vector.end(), _s) != _vector.end();
}

template<class T>
bool contains(std::vector<T> _vector, T _s)
{
    return std::find(_vector.begin(), _vector.end(), _s) != _vector.end();
}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkAnalytes(const DrugModel* _drugModel)
{
    std::vector<AnalyteId> activeMoietiesAnalytes;

    // 1. Check that there is no overlap amongst moieties
    for (const auto& activeMoiety : _drugModel->getActiveMoieties()) {
        for (const auto& analyteId : activeMoiety->getAnalyteIds()) {
            if (contains<AnalyteId>(activeMoietiesAnalytes, analyteId)) {
                return {false,
                        "The analyte " + analyteId.toString() + " is present at least twice in the active moieties"};
            }
            activeMoietiesAnalytes.push_back(analyteId);
        }
    }

    // Sort the list for further comparisons
    std::sort(activeMoietiesAnalytes.begin(), activeMoietiesAnalytes.end());


    // 2. Check that there is no overlap amongst the analyte groups
    std::vector<AnalyteId> allAnalytes;
    for (const auto& analyteSet : _drugModel->m_analyteSets) {
        for (const auto& analyte : analyteSet->getAnalytes()) {
            if (contains<AnalyteId>(allAnalytes, analyte->getAnalyteId())) {
                return {false,
                        "The analyte " + analyte->getAnalyteId().toString()
                                + " is present at least twice in the analyte groups"};
            }
            allAnalytes.push_back(analyte->getAnalyteId());
        }
    }

    std::sort(allAnalytes.begin(), allAnalytes.end());

    // 3. Check that there is an exact match between the moieties and analyte groups
    if (allAnalytes != activeMoietiesAnalytes) {
        return {false, "The analytes in the active moieties and the analyte groupes are not the same"};
    }

    // 4. Check that there is an exact match between the conversions and the moieties
    for (const auto& formulationAndRoute : _drugModel->m_formulationAndRoutes->m_fars) {

        std::vector<AnalyteId> analytes;
        for (const auto& analyteConversion : formulationAndRoute->getAnalyteConversions()) {
            analytes.push_back(analyteConversion->getAnalyteId());
        }
        std::sort(analytes.begin(), analytes.end());
        if (allAnalytes != analytes) {
            return {false,
                    "The analytes in the analyte conversions of a formulation and routes are not the same as the full set of analytes"};
        }
    }

    return {true, ""};
}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkFormulaInputs(const DrugModel* _drugModel)
{
    // First get the list of covariate Ids
    std::vector<std::string> validInputs;
    for (const auto& covariate : _drugModel->getCovariates()) {
        validInputs.push_back(covariate->getId());
    }

    // Second the disposition parameters
    for (const auto& analyteGroup : _drugModel->getAnalyteSets()) {
        for (const auto& parameter : analyteGroup->getDispositionParameters().m_parameters) {
            validInputs.push_back(parameter->getId() + "_population");
        }
    }


    // Second add the parameters
    //    for (const auto &parameter : _drugModel->getAbsorptionParameters())


    std::vector<Operation*> operations;

    getAllOperations(_drugModel, operations);

    for (const Operation* operation : operations) {
        if (operation != nullptr) {
            for (const auto& input : operation->getInputs()) {
                if (!contains(validInputs, input.getName())) {
                    return {false, "The input " + input.getName() + " is not a valid formula input"};
                }
            }
        }
    }
    return {true, ""};
}


DrugModelChecker::CheckerResult_t DrugModelChecker::checkParameters(
        const DrugModel* _drugModel, const PkModelCollection* _pkCollection)
{

    // We need a PK model for each analyte group
    for (const auto& analyteGroup : _drugModel->getAnalyteSets()) {
        std::shared_ptr<PkModel> pkModel = _pkCollection->getPkModelFromId(analyteGroup->getPkModelId());
        if (pkModel == nullptr) {
            return {false,
                    "There is no PK model corresponding to the one defined in the drug model ("
                            + analyteGroup->getPkModelId() + ")"};
        }


        const ParameterSetDefinition* dispositionParameters =
                _drugModel->getDispositionParameters(analyteGroup->getId());


        for (const auto& formulation : _drugModel->getFormulationAndRoutes()) {

            std::vector<std::string> requiredParameters =
                    pkModel->getParametersForRoute(formulation->getAbsorptionModel(analyteGroup->getId()));
            if (requiredParameters.empty()) {
                requiredParameters =
                        pkModel->getParametersForRoute(formulation->getFormulationAndRoute().getAbsorptionModel());
                if (requiredParameters.empty()) {
                    return {false,
                            Tucuxi::Common::Utils::strFormat(
                                    "It seems that there is no PK model for formulation and route %s, for analyte group %s",
                                    formulation->getId().c_str(),
                                    analyteGroup->getId().toString().c_str())};
                }
            }

            std::sort(requiredParameters.begin(), requiredParameters.end());

            const ParameterSetDefinition* absorptionParameters =
                    formulation->getParameterDefinitions(analyteGroup->getId());

            std::vector<std::string> formulationParameters;
            for (const auto& p : dispositionParameters->m_parameters) {
                formulationParameters.push_back(p->getId());
            }
            if (absorptionParameters != nullptr) {
                for (const auto& p : absorptionParameters->m_parameters) {
                    formulationParameters.push_back(p->getId());
                }
            }

            std::sort(formulationParameters.begin(), formulationParameters.end());

            if (requiredParameters.size() != formulationParameters.size()) {

                std::string reqString;
                for (const auto& requiredParameter : requiredParameters) {
                    reqString += requiredParameter + "|";
                }

                std::string forString;
                for (const auto& formulationParameter : formulationParameters) {
                    forString += formulationParameter + "|";
                }

                return {false,
                        Tucuxi::Common::Utils::strFormat(
                                "The formulation and route %s does not have the same number of parameters for the analyte group %s. Required : %s. Given : %s",
                                formulation->getId().c_str(),
                                analyteGroup->getId().toString().c_str(),
                                reqString.c_str(),
                                forString.c_str())};
            }

            for (size_t i = 0; i < requiredParameters.size(); i++) {
                if (requiredParameters[i] != formulationParameters[i]) {

                    std::string reqString;
                    for (const auto& requiredParameter : requiredParameters) {
                        reqString += requiredParameter + "|";
                    }

                    std::string forString;
                    for (const auto& formulationParameter : formulationParameters) {
                        forString += formulationParameter + "|";
                    }
                    return {false,
                            Tucuxi::Common::Utils::strFormat(
                                    "The formulation and route %s does not have the right PK parameters for the analyte group %s. Required : %s. Given : %s",
                                    formulation->getId().c_str(),
                                    analyteGroup->getId().toString().c_str(),
                                    reqString.c_str(),
                                    forString.c_str())};
                }
            }
        }
    }

    return {true, ""};
}


void DrugModelChecker::getAllOperations(const DrugModel* _drugModel, std::vector<Operation*>& _operations)
{

    for (const auto& analyteGroup : _drugModel->getAnalyteSets()) {
        for (const auto& parameter : analyteGroup->getDispositionParameters().m_parameters) {
            if (parameter->isComputed()) {
                _operations.push_back(&parameter->getOperation());
            }
        }
    }

    for (const auto& formulation : _drugModel->getFormulationAndRoutes()) {
        for (const auto& analyteGroup : _drugModel->getAnalyteSets()) {
            auto parameters = formulation->getParameterDefinitions(analyteGroup->getId());
            if (parameters != nullptr) {
                for (size_t index = 0; index < parameters->getNbParameters(); index++) {
                    const auto parameter = parameters->getParameter(index);
                    if (parameter->isComputed()) {
                        _operations.push_back(&parameter->getOperation());
                    }
                }
            }
        }
    }

    for (const auto& constraint : _drugModel->getDomain().getConstraints()) {
        _operations.push_back(constraint->getCheckOperationPointer());
    }

    for (const auto& covariate : _drugModel->getCovariates()) {
        auto validation = covariate->getValidation();
        if (validation != nullptr) {
            _operations.push_back(covariate->getValidation());
        }
    }
}


DrugModelChecker::CheckerResult_t DrugModelChecker::checkOperations(const DrugModel* _drugModel)
{

    std::vector<Operation*> operations;

    getAllOperations(_drugModel, operations);

    for (Operation* operation : operations) {
        if (operation != nullptr) {
            OperationInputList inputList;
            for (const auto& input : operation->getInputs()) {
                switch (input.getType()) {
                case InputType::BOOL: {
                    inputList.push_back(OperationInput(input.getName(), true));
                } break;
                case InputType::DOUBLE: {
                    inputList.push_back(OperationInput(input.getName(), 1.0));
                } break;
                case InputType::INTEGER: {
                    inputList.push_back(OperationInput(input.getName(), 1));
                } break;
                }
            }

            JSOperation* jsOperation = dynamic_cast<JSOperation*>(operation);
            if (jsOperation != nullptr) {
                if (!jsOperation->checkOperation(inputList)) {

                    std::string exp = jsOperation->getExpression();
                    std::stringstream sstream(exp);
                    std::string line;
                    std::string numberedExp;

                    int lineIndex = 0;
                    while (std::getline(sstream, line, '\n')) {
                        numberedExp += std::to_string(lineIndex) + " .\t" + line + "\n";
                        lineIndex++;
                    }


                    return {false,
                            "The operation could not be evaluated with input values being 1 (for double and int) and true for booleans. Maybe you forgot to set up some of the required inputs in the input list: \n\n"
                                    + numberedExp + "\n\nSpecific error message: \n"
                                    + operation->getLastErrorMessage()};
                }
            }
        }
    }
    return {true, ""};
}


DrugModelChecker::CheckerResult_t DrugModelChecker::checkOperation(Operation* _operation)
{
    if (_operation != nullptr) {
        OperationInputList inputList;
        for (const auto& input : _operation->getInputs()) {
            switch (input.getType()) {
            case InputType::BOOL: {
                inputList.push_back(OperationInput(input.getName(), true));
            } break;
            case InputType::DOUBLE: {
                inputList.push_back(OperationInput(input.getName(), 1.0));
            } break;
            case InputType::INTEGER: {
                inputList.push_back(OperationInput(input.getName(), 1));
            } break;
            }
        }

        JSOperation* jsOperation = dynamic_cast<JSOperation*>(_operation);
        if (jsOperation != nullptr) {
            if (!jsOperation->checkOperation(inputList)) {

                std::string exp = jsOperation->getExpression();
                std::stringstream sstream(exp);
                std::string line;
                std::string numberedExp;

                int lineIndex = 0;
                while (std::getline(sstream, line, '\n')) {
                    numberedExp += std::to_string(lineIndex) + " .\t" + line + "\n";
                    lineIndex++;
                }


                return {false,
                        "The operation could not be evaluated with input values being 1 (for double and int) and true for booleans. Maybe you forgot to set up some of the required inputs in the input list: \n\n"
                                + numberedExp + "\n\nSpecific error message: \n" + _operation->getLastErrorMessage()};
            }
        }
    }
    return {true, ""};
}

DrugModelChecker::CheckerResult_t DrugModelChecker::checkHalfLife(const DrugModel* _drugModel)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    // Create a time range starting at the beginning of june 2017, with no upper end (to test that the repetitions
    // are handled correctly)
    DateTime start(
            date::year_month_day(date::year(2018), date::month(1), date::day(1)),
            Duration(std::chrono::hours(8), std::chrono::minutes(0), std::chrono::seconds(0)));

    auto defaultFormulationAndRoute = _drugModel->getFormulationAndRoutes().getDefault();
    const auto& formulationAndRoute = defaultFormulationAndRoute->getFormulationAndRoute();
    auto dose = defaultFormulationAndRoute->getValidDoses()->getDefaultValue();
    auto doseUnit = defaultFormulationAndRoute->getValidDoses()->getUnit();
    auto interval = defaultFormulationAndRoute->getValidIntervals()->getDefaultDuration();
    //const FormulationAndRoute route("formulation", AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    // Add a treatment intake every ten days in June
    // 200mg via a intravascular at 08h30, starting the 01.06
    LastingDose periodicDose(dose, doseUnit, formulationAndRoute, Duration(), interval);
    DosageRepeat repeatedDose(periodicDose, 10000);
    auto timeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(start, repeatedDose);
    drugTreatment->getModifiableDosageHistory().addTimeRange(*timeRange);

    IComputingService* componentPtr = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    std::unique_ptr<IComputingService> component = std::unique_ptr<IComputingService>(componentPtr);

    {
        RequestResponseId requestResponseId = "1";

        auto halfLife = _drugModel->getTimeConsiderations().getHalfLife().getValue();
        auto unit = _drugModel->getTimeConsiderations().getHalfLife().getUnit();
        auto multiplier = _drugModel->getTimeConsiderations().getHalfLife().getMultiplier();

        Duration realHalfLife;
        if (unit.toString() == "h") {
            realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife * 60)));
        }
        else if (unit.toString() == "d") {
            realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife * 60 * 24)));
        }
        else if (unit.toString() == "min") {
            realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife)));
        }
        else {
            return {false, R"(The half life unit should be "h", "d" or "min")"};
        }


        Tucuxi::Common::DateTime startPred = start + realHalfLife * multiplier;

        Tucuxi::Common::DateTime endPred = startPred + interval * 2;
        double nbPointsPerHour = 10.0;
        ComputingOption computingOption(PredictionParameterType::Population, CompartmentsOption::MainCompartment);
        std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                requestResponseId, startPred, endPred, nbPointsPerHour, computingOption);

        ComputingRequest request(requestResponseId, *_drugModel, *drugTreatment, std::move(traits));

        std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

        ComputingStatus result;
        result = component->compute(request, response);

        if (result != ComputingStatus::Ok) {
            return {false,
                    "A prediction calculation based on the halflife and the multiplier went wrong. Please check these values."};
        }

        const ComputedData* responseData = response->getData();
        const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

        MultiCompConcentration residuals;
        for (const auto& d : resp->getData()) {
            residuals.push_back(d.m_concentrations[0][0]);
        }

        auto diff = residuals[0] / residuals[1];

        // If less than 0.5% difference, then the values seem OK
        if ((diff > 1.005) || (diff < 0.995)) {


            {
                RequestResponseId requestResponseId = "1";

                auto halfLife = _drugModel->getTimeConsiderations().getHalfLife().getValue();
                auto unit = _drugModel->getTimeConsiderations().getHalfLife().getUnit();

                Duration realHalfLife;
                if (unit.toString() == "h") {
                    realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife * 60)));
                }
                else if (unit.toString() == "d") {
                    realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife * 60 * 24)));
                }
                else if (unit.toString() == "min") {
                    realHalfLife = Duration(std::chrono::minutes(static_cast<int>(halfLife)));
                }
                else {
                    return {false, R"(The half life unit should be "h", "d" or "min")"};
                }


                Tucuxi::Common::DateTime startPred = start;

                Tucuxi::Common::DateTime endPred = startPred + interval * 500;
                double nbPointsPerHour = 0.1;
                ComputingOption computingOption(
                        PredictionParameterType::Population, CompartmentsOption::MainCompartment);
                std::unique_ptr<ComputingTraitConcentration> traits = std::make_unique<ComputingTraitConcentration>(
                        requestResponseId, startPred, endPred, nbPointsPerHour, computingOption);

                ComputingRequest request(requestResponseId, *_drugModel, *drugTreatment, std::move(traits));

                std::unique_ptr<ComputingResponse> response = std::make_unique<ComputingResponse>(requestResponseId);

                ComputingStatus result;
                result = component->compute(request, response);

                if (result != ComputingStatus::Ok) {
                    return {false,
                            "A prediction calculation based on the halflife and the multiplier went wrong. Please check these values."};
                }

                const ComputedData* responseData = response->getData();
                const SinglePredictionData* resp = dynamic_cast<const SinglePredictionData*>(responseData);

                MultiCompConcentration residuals;
                double lastResidual = 10000000.0;
                int index = 0;
                for (const auto& d : resp->getData()) {
                    auto residual = d.m_concentrations[0][0];
                    auto diff = residual / lastResidual;
                    lastResidual = residual;
                    // If less than 0.5% difference, then the values seem OK
                    if ((diff < 1.005) && (diff > 0.995)) {

                        Tucuxi::Common::Duration duration = interval * index;
                        int factor = static_cast<int>(std::ceil(duration / realHalfLife));

                        return {false,
                                "The half life and the associated multiplier offer a too short period to reach a reasonable steady state. Please modify the multiplier. As far as I could try, the value "
                                        + std::to_string(factor) + " should be all right."};
                    }

                    index++;
                }


                // If less than 0.5% difference, then the values seem OK
                if ((diff > 1.005) || (diff < 0.995)) {

                    return {false,
                            "The half life and the associated multiplier offer a too short period to reach a reasonable steady state. However I couldn't find a good multiplier to suggest. Is there another problem with the model?"};
                }
            }


            return {false,
                    "The half life and the associated multiplier offer a too short period to reach a reasonable steady state. Please modify the multiplier."};
        }
    }

    return {true, ""};
}


} // namespace Core
} // namespace Tucuxi
