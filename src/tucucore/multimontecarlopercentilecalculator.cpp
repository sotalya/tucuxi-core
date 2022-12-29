//@@license@@

#include <algorithm>
#include <mutex>
#include <random>
#include <thread>
#include <time.h>

#include <Eigen/Cholesky>

#include "multimontecarlopercentilecalculator.h"

#include "concentrationcalculator.h"
#include "definitions.h"
#include "drugmodel/activemoiety.h"
#include "percentilesprediction.h"

namespace Tucuxi {
namespace Core {


ComputingStatus AprioriPercentileCalculatorMulti::calculate(
        PercentilesPrediction& _percentiles,
        const DateTime& _recordFrom,
        const DateTime& _recordTo,
        const GroupsIntakeSeries& _intakes,
        const GroupsParameterSetSeries& _parameters,
        const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
        const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
        const std::map<AnalyteGroupId, Etas>& _etas,
        const PercentileRanks& _percentileRanks,
        IConcentrationCalculator& _concentrationCalculator,
        const ActiveMoiety* _activeMoiety,
        ComputingAborter* _aborter)
{
    AprioriMonteCarloPercentileCalculator simpleCalculator;

    size_t nbPatients = Tucuxi::Core::MonteCarloPercentileCalculatorBase::getNumberPatients();
    std::vector<TimeOffsets> times;

    IntakeSeries recordedIntakes;

    std::vector<AnalyteGroupId> analyteGroupIds;

    for (const auto& parameter : _parameters) {
        analyteGroupIds.push_back(parameter.first);
    }

    std::vector<std::vector<std::vector<std::vector<Concentration> > > > concentrations;

    size_t analyteGroupIndex = 0;
    for (const auto& analyteGroupId : analyteGroupIds) {
        // As computePredictions() will fill recordedIntakes we have to be careful
        // not to fill them more than once. Therefore we clear it first
        recordedIntakes.clear();
        std::vector<Etas> etaSamples;
        std::vector<Deviations> epsilons;
        ComputingStatus result = simpleCalculator.calculateEtasAndEpsilons(
                etaSamples,
                epsilons,
                _omega.at(analyteGroupId),
                *_residualErrorModel.at(analyteGroupId),
                _etas.at(analyteGroupId));

        if (result != ComputingStatus::Ok) {
            return result;
        }
        concentrations.push_back(std::vector<std::vector<std::vector<Concentration> > >(0));

        result = simpleCalculator.computePredictions(
                _recordFrom,
                _recordTo,
                _intakes.at(analyteGroupId),
                _parameters.at(analyteGroupId),
                *_residualErrorModel.at(analyteGroupId),
                etaSamples,
                epsilons,
                _concentrationCalculator,
                nbPatients,
                times,
                recordedIntakes,
                concentrations[analyteGroupIndex],
                _aborter);
        if (result != ComputingStatus::Ok) {
            return result;
        }

        analyteGroupIndex++;
    }

    return calculateActiveMoietyAndSort(
            _percentiles,
            _percentileRanks,
            _activeMoiety,
            analyteGroupIds,
            recordedIntakes,
            nbPatients,
            concentrations,
            simpleCalculator,
            times);
}

ComputingStatus AposterioriMonteCarloPercentileCalculatorMulti::calculate(
        PercentilesPrediction& _percentiles,
        const DateTime& _recordFrom,
        const DateTime& _recordTo,
        const GroupsIntakeSeries& _intakes,
        const GroupsParameterSetSeries& _parameters,
        const std::map<AnalyteGroupId, OmegaMatrix>& _omega,
        const std::map<AnalyteGroupId, std::unique_ptr<IResidualErrorModel> >& _residualErrorModel,
        const std::map<AnalyteGroupId, Etas>& _etas,
        const std::map<AnalyteGroupId, SampleSeries>& _samples,
        const PercentileRanks& _percentileRanks,
        IConcentrationCalculator& _concentrationCalculator,
        const ActiveMoiety* _activeMoiety,
        ComputingAborter* _aborter)
{
    AposterioriMonteCarloPercentileCalculator simpleCalculator;

    size_t nbPatients = Tucuxi::Core::MonteCarloPercentileCalculatorBase::getNumberPatients();
    std::vector<TimeOffsets> times;

    IntakeSeries recordedIntakes;

    std::vector<AnalyteGroupId> analyteGroupIds;

    for (const auto& parameter : _parameters) {
        analyteGroupIds.push_back(parameter.first);
    }

    std::vector<std::vector<std::vector<std::vector<Concentration> > > > concentrations;

    size_t analyteGroupIndex = 0;
    for (const auto& analyteGroupId : analyteGroupIds) {
        // As computePredictions() will fill recordedIntakes we have to be careful
        // not to fill them more than once. Therefore we clear it first
        recordedIntakes.clear();
        std::vector<Etas> etaSamples;
        std::vector<Deviations> epsilons;
        ComputingStatus result = simpleCalculator.calculateEtasAndEpsilons(
                etaSamples,
                epsilons,
                _intakes.at(analyteGroupId),
                _parameters.at(analyteGroupId),
                _omega.at(analyteGroupId),
                *_residualErrorModel.at(analyteGroupId),
                _etas.at(analyteGroupId),
                _samples.at(analyteGroupId),
                _concentrationCalculator,
                _aborter);

        if (result != ComputingStatus::Ok) {
            return result;
        }
        concentrations.push_back(std::vector<std::vector<std::vector<Concentration> > >(0));

        result = simpleCalculator.computePredictions(
                _recordFrom,
                _recordTo,
                _intakes.at(analyteGroupId),
                _parameters.at(analyteGroupId),
                *_residualErrorModel.at(analyteGroupId),
                etaSamples,
                epsilons,
                _concentrationCalculator,
                nbPatients,
                times,
                recordedIntakes,
                concentrations[analyteGroupIndex],
                _aborter);
        if (result != ComputingStatus::Ok) {
            return result;
        }

        analyteGroupIndex++;
    }

    return calculateActiveMoietyAndSort(
            _percentiles,
            _percentileRanks,
            _activeMoiety,
            analyteGroupIds,
            recordedIntakes,
            nbPatients,
            concentrations,
            simpleCalculator,
            times);
}

ComputingStatus PercentileCalculatorMultiBase::calculateActiveMoietyAndSort(
        PercentilesPrediction& _percentiles,
        const PercentileRanks& _percentileRanks,
        const ActiveMoiety* _activeMoiety,
        const std::vector<AnalyteGroupId>& _analyteGroupIds,
        IntakeSeries _recordedIntakes,
        size_t _nbPatients,
        std::vector<std::vector<std::vector<std::vector<Concentration> > > >& _concentrations,
        MonteCarloPercentileCalculatorBase& _simpleCalculator,
        const std::vector<TimeOffsets>& _times)
{

    if ((_activeMoiety != nullptr) && (_activeMoiety->getFormula() != nullptr)
        && (_activeMoiety->getAnalyteIds().size() > 1)) {

        Operation* op = _activeMoiety->getFormula();

        std::vector<std::vector<std::vector<Concentration> > > aConcentration;


        // Set the size of vector "concentrations"
        for (auto& recordedIntake : _recordedIntakes) {
            std::vector<std::vector<Concentration> > vec;
            for (size_t point = 0; point < recordedIntake.getNbPoints(); point++) {
                vec.push_back(std::vector<Concentration>(_nbPatients));
            }
            aConcentration.push_back(vec);
        }

        OperationInputList inputList;
        for (size_t i = 0; i < _analyteGroupIds.size(); i++) {
            inputList.push_back(OperationInput("input" + std::to_string(i), 0.0));
        }

        size_t nbAnalyteGroups = _analyteGroupIds.size();

        // TODO : This loop could be multi-threaded
        size_t size1 = _concentrations[0].size();
        for (size_t i = 0; i < size1; i++) {
            size_t size2 = _concentrations[0][i].size();
            for (size_t j = 0; j < size2; j++) {
                size_t size3 = _concentrations[0][i][j].size();
                for (size_t k = 0; k < size3; k++) {

                    for (size_t index = 0; index < nbAnalyteGroups; index++) {
                        inputList[index].setValue(_concentrations[index][i][j][k]);
                    }
                    double result;
                    if (!op->evaluate(inputList, result)) {
                        // Something went wrong
                        return ComputingStatus::ActiveMoietyCalculationError;
                    }
                    aConcentration[i][j][k] = result;
                }
            }
        }

        return _simpleCalculator.sortAndExtractPercentiles(
                _percentiles, _percentileRanks, _nbPatients, _times, _recordedIntakes, aConcentration);
    }
    else {
        return _simpleCalculator.sortAndExtractPercentiles(
                _percentiles, _percentileRanks, _nbPatients, _times, _recordedIntakes, _concentrations[0]);
    }
}


} // namespace Core
} // namespace Tucuxi
