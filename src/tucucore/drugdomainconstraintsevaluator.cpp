//@@license@@

#include "drugdomainconstraintsevaluator.h"

#include "tucucore/covariateevent.h"
#include "tucucore/covariateextractor.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugtreatment/patientcovariate.h"

namespace Tucuxi {
namespace Core {

DrugDomainConstraintsEvaluator::DrugDomainConstraintsEvaluator() = default;


DrugDomainConstraintsEvaluator::Result DrugDomainConstraintsEvaluator::evaluate(
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment,
        const DateTime& _start,
        const DateTime& _end,
        std::vector<DrugDomainConstraintsEvaluator::EvaluationResult>& _results)
{
    // 1. Generate a CovariateSeries corresponding to the [_start, _end] interval

    // 2. For each constraint, evaluate it on the entire CovariateSeries

    CovariateSeries covariateSeries;


    CovariateExtractor extractor(_drugModel.getCovariates(), _drugTreatment.getCovariates(), _start, _end);

    auto status = extractor.extract(covariateSeries);
    if (status != ComputingStatus::Ok) {
        return DrugDomainConstraintsEvaluator::Result::ComputationError;
    }

    DrugDomainConstraintsEvaluator::Result result =
            evaluate(_drugModel.getDomain(), covariateSeries, _start, _end, _results);


    //Change result if hard mandatory constraint and covariate corresponding not in drug treatment
    bool equal = false;
    for (auto& res : _results) {
        if (res.m_constraint->getType() == ConstraintType::MANDATORYHARD) {
            for (const auto& dtCovariate : _drugTreatment.getCovariates()) {
                if (res.m_constraint->getRequiredCovariateIds()[0] == dtCovariate->getId()) {
                    equal = true;
                }
                else if (
                        res.m_constraint->getRequiredCovariateIds()[0] == "age"
                        || res.m_constraint->getRequiredCovariateIds()[0] == "Age") {
                    if (dtCovariate->getId() == "Birthdate" || dtCovariate->getId() == "birthdate") {
                        equal = true;
                    }
                }
            }
            if (!equal) {
                res.m_result = Result::Incompatible;
                result = Result::Incompatible;
            }
            equal = false;
        }
    }

    return result;
}

DrugDomainConstraintsEvaluator::Result DrugDomainConstraintsEvaluator::evaluate(
        const DrugModelDomain& _drugDomain,
        const CovariateSeries& _covariates,
        const DateTime& _start,
        const DateTime& _end,
        std::vector<EvaluationResult>& _results)
{
    /// \brief Collection of timed covariate events, used to build the set of parameters.
    std::map<DateTime, std::vector<std::pair<std::string, Value>>> timedCValues;


    // All the following lines, up to     // Ready for computation
    // are the same as the ones in ParameterExtractor. We can think about some refactoring


    // Check that start time is past end time.
    if (_start > _end) {
        throw std::runtime_error("[ParametersExtractor] Invalid time interval specified");
    }

    // Create a list of time instants at which to compute the parameters, together with the covariate values that have
    // to be set at those instants.
    for (const auto& cv : _covariates) {
        const DateTime dt = cv.getEventTime();

        // Covariate events past _end are discarded.
        if (dt <= _end) {
            // Allocate the vector if not yet present at the given time instant.
            if (timedCValues.find(dt) == timedCValues.end()) {
                timedCValues.insert(std::make_pair(dt, std::vector<std::pair<std::string, Value>>()));
            }
            timedCValues.at(dt).push_back(std::make_pair(cv.getId(), cv.getValue()));
        }
    }

    // Allocate the vector at _start if no event present. Indeed, even if no covariate plays an influence on the
    // parameters, the parameters have to be determined at _start.
    if (timedCValues.empty()) {
        timedCValues.insert(std::make_pair(_start, std::vector<std::pair<std::string, Value>>()));
    }

    // std::maps are sorted on the key value, so we can safely assume that the first value is the first time interval
    // considered.
    std::vector<DateTime> toErase;
    DateTime firstEvTime = timedCValues.begin()->first;
    if (firstEvTime < _start) {
        // If we have values before _start, we want _start to be present to catch their first initializations.
        timedCValues.insert(std::make_pair(_start, std::vector<std::pair<std::string, Value>>()));

        // This is the vector we seek to fill with the first values of the covariate events.
        std::vector<std::pair<std::string, Value>>& startCVs = timedCValues.at(_start);
        // Propagate forward the covariates whose events happen before _start.
        for (auto& tcVec : timedCValues) {
            if (tcVec.first >= _start) {
                // We can stop as soon as we reach _start.
                break;
            }
            for (auto& tcVal : tcVec.second) {
                // If the covariate is not present, then simply add it. If it is present, then change its value to a
                // more up-to-date one.
                auto it = std::find_if(
                        startCVs.begin(), startCVs.end(), [tcVal](const std::pair<std::string, Value>& _event) {
                            return _event.first == tcVal.first;
                        });
                if (it == startCVs.end()) {
                    startCVs.push_back(tcVal);
                }
                else {
                    it->second = tcVal.second;
                }
            }
            // Add the time instant to the list of time instants to remove from the map.
            toErase.push_back(tcVec.first);
        }
    }
    // Clean up the set of time intervals.
    for (const auto& rm : toErase) {
        timedCValues.erase(rm);
    }

    if (timedCValues.empty()) {
        // Check for covariates that appear out of nowhere past the start time -- if any other covariate was present
        // before, then that was the initial time of parameters computation and all the covariates must be defined at
        // that moment.
        std::vector<std::string> knownCovariates;
        for (const auto& startCV : (timedCValues.begin())->second) {
            knownCovariates.push_back(startCV.first);
        }

        // First vector checked twice, but the algorithm looks more clear in this way.
        for (auto& tcVec : timedCValues) {
            for (auto& tcVal : tcVec.second) {
                if (std::find(knownCovariates.begin(), knownCovariates.end(), tcVal.first) == knownCovariates.end()) {
                    throw std::runtime_error(
                            "[ParametersExtractor] Covariate event for " + tcVal.first + " comes out blue sky!");
                }
            }
            // Since we are already passing on individual dates, check that the covariates have no duplicate definition
            // (that is, for a given time instant, two possible values).
            std::sort(
                    tcVec.second.begin(),
                    tcVec.second.end(),
                    [](const std::pair<std::string, Value>& _a, const std::pair<std::string, Value>& _b) {
                        return _a.first < _b.first;
                    });
            // We consider wrong to have two covariate events referring to the same covariate at the same time instant
            // -- even if the value is the same, as this is possibly a mistake of the algorithm that extracted covariate
            // events.
            if (std::adjacent_find(
                        tcVec.second.begin(),
                        tcVec.second.end(),
                        [](const std::pair<std::string, Value>& _a, const std::pair<std::string, Value>& _b) {
                            return _a.first == _b.first;
                        })
                != tcVec.second.end()) {
                throw std::runtime_error("[ParametersExtractor] Duplicate values for the same covariate event.");
            }
        }
    }

    // Ready for computation


    // By default, the result should indicate compabitility
    Result resultStatus = Result::Compatible;

    // Map linking covariate events with their current value.
    std::map<std::string, double> cEvMap;

    // Iterate over the constraints
    for (const auto& constraint : _drugDomain.getConstraints()) {
        std::unique_ptr<Operation> operation = constraint->getCheckOperation().clone();

        // By default, the result should indicate compabitility
        Result constraintResultStatus = Result::Compatible;


        // Iterate on the time instants and calculate the evaluation function. Particular handling is required by the events at
        // the beginning of the considered interval, but it is easier to do this inside the loop.
        for (const auto& tcv : timedCValues) {

            OperationInputList inputList;

            if (tcv.first == timedCValues.begin()->first) {
                // Add all covariates at the first time instant as inputs of the OGM.
                for (const auto& cv : tcv.second) {
                    cEvMap.insert(std::make_pair(cv.first, cv.second));
                }
            }
            else {
                // Just set the covariate values that are scheduled at this time instant.
                for (const auto& cv : tcv.second) {
                    cEvMap[cv.first] = cv.second;
                }
            }

            // Build the input list by getting values from the map
            for (auto& kv : cEvMap) {
                inputList.push_back(OperationInput(kv.first, kv.second));
            }



            double result = NAN;
            bool status = operation->evaluate(inputList, result);

            // If an error occured, the result will indicate this
            if (!status) {
                resultStatus = Result::ComputationError;
                constraintResultStatus = Result::ComputationError;
            }
            // If the result is false and there is no previous computation error, then update the result status
            if (result == 0.0) {

                if (constraintResultStatus != Result::ComputationError) {
                    if (constraintResultStatus != Result::Incompatible) {
                        if (constraint->getType() == ConstraintType::SOFT) {
                            constraintResultStatus = Result::PartiallyCompatible;
                        }
                        else {
                            constraintResultStatus = Result::Incompatible;
                        }
                    }
                }

                if (resultStatus != Result::ComputationError) {
                    if (resultStatus != Result::Incompatible) {
                        if (constraint->getType() == ConstraintType::SOFT) {
                            resultStatus = Result::PartiallyCompatible;
                        }
                        else {
                            resultStatus = Result::Incompatible;
                        }
                    }
                }
            }
        }
        DrugDomainConstraintsEvaluator::EvaluationResult evaluationResult;
        evaluationResult.m_result = constraintResultStatus;
        evaluationResult.m_constraint = constraint.get();
        _results.push_back(evaluationResult);
    }

    return resultStatus;
}


} // namespace Core
} // namespace Tucuxi
