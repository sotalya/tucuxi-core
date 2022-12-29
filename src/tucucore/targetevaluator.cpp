//@@license@@

#include "targetevaluator.h"

#include "tucucore/concentrationprediction.h"
#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/intakeevent.h"
#include "tucucore/targetevaluationresult.h"

using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

TargetEvaluator::TargetEvaluator() = default;


bool TargetEvaluator::isWithinBoundaries(Value _value, Value _min, Value _max)
{
    // We tolerate a small deviation from the min and max, to account for
    // floating point calculations.
    Value delta = 1e-7;
    return (_value > _min - delta) && (_value <= _max + delta);
}

void TargetEvaluator::evaluateValue(
        Value _value, const TargetEvent& _targetEvent, bool& _ok, double& _score, double& _outputValue)
{
    _ok &= isWithinBoundaries(_value, _targetEvent.m_valueMin, _targetEvent.m_valueMax);

    if (_ok) {
        _score = 1.0
                 - pow(log(_value) - log(_targetEvent.m_valueBest), 2)
                           / pow(0.5 * (log(_targetEvent.m_valueMax) - log(_targetEvent.m_valueMin)), 2);
        _outputValue = _value;
    }
}

double TargetEvaluator::aucOverMicCalculator(const TargetEvent& _targetEvent, CycleData& _cycle)
{
    double aucOverMic = 0.0;
    double timeOverMic = 0.0;
    Value mic = _targetEvent.m_mic;
    Concentrations concentrations =
            _cycle.getConcentrations()[0]; // [0] because only one concentrations (vector of concentration) in CycleData
    for (size_t i = 0; i < concentrations.size(); i++) {

        if (concentrations[i] > mic) {
            if (i > 0) {
                if (concentrations[i - 1] < mic) {
                    double deltaX = _cycle.getTimes()[0][i] - _cycle.getTimes()[0][i - 1];
                    double difference = concentrations[i] - concentrations[i - 1];
                    double coefficient = (concentrations[i] - mic) / difference;
                    timeOverMic = deltaX * coefficient;
                    aucOverMic += ((concentrations[i] - mic) * timeOverMic) / 2; //Triangle Area
                }
                else {
                    aucOverMic += ((concentrations[i - 1] - mic + concentrations[i] - mic)
                                   * (_cycle.getTimes()[0][i] - _cycle.getTimes()[0][i - 1]))
                                  / 2; // Trapeze area
                }
            }
        }
        else if (concentrations[i] == mic) {
            if (i > 0) {
                if (concentrations[i - 1] > mic) {
                    aucOverMic +=
                            ((concentrations[i - 1] - mic) * (_cycle.getTimes()[0][i] - _cycle.getTimes()[0][i - 1]))
                            / 2; //Triangle Area
                }
            }
        }
        else {
            if (i > 0) {
                if (concentrations[i - 1] > mic) {
                    double deltaX = _cycle.getTimes()[0][i] - _cycle.getTimes()[0][i - 1];
                    double difference = concentrations[i - 1] - concentrations[i];
                    double coefficient = (concentrations[i - 1] - mic) / difference;
                    timeOverMic = deltaX * coefficient;
                    aucOverMic += ((concentrations[i - 1] - mic) * timeOverMic) / 2; //Triangle Area
                }
            }
        }
    }
    return aucOverMic;
}

ComputingStatus TargetEvaluator::evaluate(
        const ConcentrationPrediction& _prediction,
        const IntakeSeries& _intakeSeries,
        const TargetEvent& _targetEvent,
        TargetEvaluationResult& _result)
{

    if (_prediction.getTimes().empty()) {
        return ComputingStatus::TargetEvaluationError;
    }

    bool bOk = true;
    double score = 0.0;
    double value = 0.0;

    std::size_t lastCycleIndex = _prediction.getTimes().size() - 1;
    TimeOffsets times = _prediction.getTimes()[lastCycleIndex];
    DateTime start = _intakeSeries[lastCycleIndex].getEventTime();
    DateTime end = start + std::chrono::milliseconds(static_cast<long long>(times.back()) * 1000);
    CycleData cycle(start, end, TucuUnit("ug/l"));
    cycle.addData(times, _prediction.getValues()[lastCycleIndex]);

    // TODO : Here we only take one compartment... To be checked

    // Only valid for a single cycle, so 0.0 as cumulative AUC here.
    // It cannot be used as is for cumulative AUC of multiple cycles.
    std::vector<Value> fakeCumulativeAuc(1, 0.0);
    CycleStatistics statisticsCalculator(cycle, fakeCumulativeAuc);
    DateTime dateTime;
    std::vector<std::vector<Tucuxi::Core::CycleStatistic> > stats;

    switch (_targetEvent.m_targetType) {
    case TargetType::Peak: {
        double peakConcentration = 0.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Peak);
        if (!cycleStatistic.getValue(dateTime, peakConcentration)) {
            // Something wrong here
        }

        double peakTime = (dateTime - cycleStatistic.getCycleStartDate()).toMinutes();

        bOk = isWithinBoundaries(peakTime, _targetEvent.m_tMin.toMinutes(), _targetEvent.m_tMax.toMinutes());
        if (bOk) {
            evaluateValue(peakConcentration, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::Residual: {
        // Take compartment 0, could be different in the future
        //double lastResidual = cycle.m_concentrations[0].back();
        // We have two options, I choose the cycle statistic calculator here$
        double lastResidual = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Residual);
        bOk = cycleStatistic.getValue(dateTime, lastResidual);

        if (bOk) {
            evaluateValue(lastResidual, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::Auc: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        if (bOk) {
            evaluateValue(auc, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::Auc24: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);


        if (bOk) {
            // Calculate the AUC on 24h
            double auc24 = auc * 24.0 / intervalInHours;

            evaluateValue(auc24, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::CumulativeAuc: {
        // TODO : We only take one compartment: to be checked
        std::vector<double> cumulativeAuc(1, 0.0);

        //Tucuxi::Core::CycleStatistics stats(cycleData, cumulativeAuc);
        for (std::size_t i = 0; i < _prediction.getTimes().size(); i++) {
            TimeOffsets times = _prediction.getTimes()[i];
            DateTime start = _intakeSeries[i].getEventTime();
            DateTime end = start + std::chrono::milliseconds(static_cast<long long>(times.back()) * 1000);
            CycleData cycle(start, end, TucuUnit("ug/l"));
            cycle.addData(times, _prediction.getValues()[i]);

            // The constructor accumulates in cumulativeAuc.
            CycleStatistics statisticsCalculator(cycle, cumulativeAuc);
        }

        evaluateValue(cumulativeAuc[0], _targetEvent, bOk, score, value);
    } break;

    case TargetType::Mean: {
        double mean = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Mean);
        bOk = cycleStatistic.getValue(dateTime, mean);

        if (bOk) {
            evaluateValue(mean, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::AucDividedByMic: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);


        if (bOk) {
            // Calculate the AUC
            double aucDividedByMic = auc / _targetEvent.m_mic;

            evaluateValue(aucDividedByMic, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::Auc24DividedByMic: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the AUC on 24h
            double auc24 = auc * 24.0 / intervalInHours;
            double aucDividedByMic = auc24 / _targetEvent.m_mic;

            evaluateValue(aucDividedByMic, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::AucOverMic: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the value between AUC and MIC
            double aucOverMic = aucOverMicCalculator(_targetEvent, cycle);

            evaluateValue(aucOverMic, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::Auc24OverMic: {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the value between AUC and MIC on 24h
            double aucOverMic = aucOverMicCalculator(_targetEvent, cycle);
            double auc24OverMic = aucOverMic * 24.0 / intervalInHours;

            evaluateValue(auc24OverMic, _targetEvent, bOk, score, value);
        }
    } break;

    case TargetType::TimeOverMic: {

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        //intervalInHours vaut la dernièe valeur des TimeOffsets (47.5)

        if (bOk) {
            // Calculate the time when over mic
            double timeOverMic = 0.0;
            Concentrations concentrations =
                    cycle.getConcentrations()
                            [0]; // [0] because only one concentrations (vector of concentration) in CycleData
            for (size_t i = 0; i < concentrations.size(); i++) {

                if (concentrations[i] > _targetEvent.m_mic) {
                    if (i == 0) {
                        timeOverMic += cycle.getTimes()[0][i]; // needed if time not starting at 0
                    }
                    else {

                        if (concentrations[i - 1] < _targetEvent.m_mic) {
                            double deltaX = cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                            double difference = concentrations[i] - concentrations[i - 1];
                            double coefficient = (concentrations[i] - _targetEvent.m_mic) / difference;
                            timeOverMic += deltaX * coefficient;
                        }
                        else {
                            timeOverMic += cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                        }
                    }
                }
                else if (concentrations[i] == _targetEvent.m_mic) {
                    if (i > 0) {
                        if (concentrations[i - 1] > _targetEvent.m_mic) {
                            timeOverMic += cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                        }
                    }
                }
                else {
                    if (i > 0) {
                        if (concentrations[i - 1] > _targetEvent.m_mic) {
                            double deltaX = cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                            double difference = concentrations[i - 1] - concentrations[i];
                            double coefficient = (concentrations[i - 1] - _targetEvent.m_mic) / difference;
                            timeOverMic += deltaX * coefficient;
                        }
                    }
                }
            }

            evaluateValue(timeOverMic, _targetEvent, bOk, score, value);
        }

    } break;

    case TargetType::FractionTimeOverMic: {

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval =
                statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        //intervalInHours vaut la dernièe valeur des TimeOffsets (47.5)

        if (bOk) {
            // Calculate the time when over mic
            double timeOverMic = 0.0;
            Concentrations concentrations =
                    cycle.getConcentrations()
                            [0]; // [0] because only one concentrations (vector of concentration) in CycleData
            for (size_t i = 0; i < concentrations.size(); i++) {

                if (concentrations[i] > _targetEvent.m_mic) {
                    if (i == 0) {
                        timeOverMic += cycle.getTimes()[0][i]; // needed if time not starting at 0
                    }
                    else {

                        if (concentrations[i - 1] < _targetEvent.m_mic) {
                            double deltaX = cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                            double difference = concentrations[i] - concentrations[i - 1];
                            double coefficient = (concentrations[i] - _targetEvent.m_mic) / difference;
                            timeOverMic += deltaX * coefficient;
                        }
                        else {
                            timeOverMic += cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                        }
                    }
                }
                else if (concentrations[i] == _targetEvent.m_mic) {
                    if (i > 0) {
                        if (concentrations[i - 1] > _targetEvent.m_mic) {
                            timeOverMic += cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                        }
                    }
                }
                else {
                    if (i > 0) {
                        if (concentrations[i - 1] > _targetEvent.m_mic) {
                            double deltaX = cycle.getTimes()[0][i] - cycle.getTimes()[0][i - 1];
                            double difference = concentrations[i - 1] - concentrations[i];
                            double coefficient = (concentrations[i - 1] - _targetEvent.m_mic) / difference;
                            timeOverMic += deltaX * coefficient;
                        }
                    }
                }
            }

            double fractionTimeOverMic = timeOverMic / intervalInHours;

            evaluateValue(fractionTimeOverMic, _targetEvent, bOk, score, value);
        }

    } break;

    case TargetType::PeakDividedByMic: {
        double peakConcentration = 0.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Peak);
        if (!cycleStatistic.getValue(dateTime, peakConcentration)) {
            // Something wrong here
        }

        double peakTime = (dateTime - cycleStatistic.getCycleStartDate()).toMinutes();

        bOk = isWithinBoundaries(peakTime, _targetEvent.m_tMin.toMinutes(), _targetEvent.m_tMax.toMinutes());
        if (bOk) {

            double peakDividedByMic = peakConcentration / _targetEvent.m_mic;

            evaluateValue(peakDividedByMic, _targetEvent, bOk, score, value);
        }

    } break;

    case TargetType::ResidualDividedByMic: {
        double residualConcentration = 0.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Residual);
        bOk = cycleStatistic.getValue(dateTime, residualConcentration);

        if (bOk) {

            double peakDividedByMic = residualConcentration / _targetEvent.m_mic;

            evaluateValue(peakDividedByMic, _targetEvent, bOk, score, value);
        }

    } break;

    case TargetType::UnknownTarget: {
        return ComputingStatus::TargetEvaluationError;

    } break;
    }

    if (!bOk) {
        return ComputingStatus::InvalidCandidate;
    }

    // We build the result, as there was no error
    _result = TargetEvaluationResult(
            _targetEvent.m_targetType,
            score,
            UnitManager::convertToUnit(value, _targetEvent.m_unit, _targetEvent.m_finalUnit),
            _targetEvent.m_finalUnit);

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
