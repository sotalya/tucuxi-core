#include "targetevaluator.h"

#include "tucucore/cyclestatisticscalculator.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/intakeevent.h"
#include "tucucore/dosage.h"
#include "tucucore/drugtreatment/target.h"
#include "tucucore/targetevaluationresult.h"

namespace Tucuxi {
namespace Core {

TargetEvaluator::TargetEvaluator()
= default;

typedef struct{

    std::vector<double> timeIntervals;
    std::vector<bool> timeFound;
    int nbCompleteInterval;
//    double temporaryConcentration;
//    bool concentrationEqualMic;


}TimeStruct;


bool TargetEvaluator::isWithinBoundaries(
        Value _value,
        Value _min,
        Value _max)
{
    // We tolerate a small deviation from the min and max, to account for
    // floating point calculations.
    Value delta = 1e-7;
    return (_value > _min - delta) && (_value <= _max + delta);
}

void TargetEvaluator::evaluateValue(
        Value _value,
        const Target &_target,
        bool &_ok,
        double &_score,
        double &_outputValue)
{
    _ok &= isWithinBoundaries(_value, _target.m_valueMin, _target.m_valueMax);

    if (_ok) {
        _score = 1.0 - pow(log(_value) - log(_target.m_valueBest), 2)
                / pow(0.5 *(log(_target.m_valueMax) - log(_target.m_valueMin)), 2);
        _outputValue = _value;
    }
}

ComputingStatus TargetEvaluator::evaluate(
        const ConcentrationPrediction& _prediction,
        const IntakeSeries& _intakeSeries,
        const Target &_target,
        TargetEvaluationResult & _result)
{

    assert(!_prediction.getTimes().empty());

    bool bOk = true;
    double score = 0.0;
    double value = 0.0;

    std::size_t lastCycleIndex = _prediction.getTimes().size() - 1;
    TimeOffsets times = _prediction.getTimes()[lastCycleIndex];
    DateTime start = _intakeSeries[lastCycleIndex].getEventTime();
    DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000);
    CycleData cycle(start, end, Unit("ug/l"));
    cycle.addData(times, _prediction.getValues()[lastCycleIndex]);

    // TODO : Here we only take one compartment... To be checked

    // Only valid for a single cycle, so 0.0 as cumulative AUC here.
    // It cannot be used as is for cumulative AUC of multiple cycles.
    std::vector<Value> fakeCumulativeAuc(1, 0.0);
    CycleStatistics statisticsCalculator(cycle, fakeCumulativeAuc);
    DateTime dateTime;
    std::vector< std::vector<Tucuxi::Core::CycleStatistic> > stats;

    switch( _target.m_targetType) {
    case TargetType::Peak :
    {
        double peakConcentration = 0.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Peak);
        if (!cycleStatistic.getValue(dateTime, peakConcentration)) {
            // Something wrong here
        }

        double peakTime = (dateTime - cycleStatistic.getCycleStartDate()).toMinutes();

        bOk = isWithinBoundaries(peakTime, _target.m_tMin.toMinutes(), _target.m_tMax.toMinutes());
        if (bOk) {
            evaluateValue(peakConcentration, _target, bOk, score, value);
        }
    } break;

    case TargetType::Residual :
    {
        // Take compartment 0, could be different in the future
        //double lastResidual = cycle.m_concentrations[0].back();
        // We have two options, I choose the cycle statistic calculator here
        double lastResidual = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Residual);
        bOk = cycleStatistic.getValue(dateTime, lastResidual);

        if (bOk) {
            evaluateValue(lastResidual, _target, bOk, score, value);
        }
    } break;

    case TargetType::Auc :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        if (bOk) {
            evaluateValue(auc, _target, bOk, score, value);
        }
    } break;

    case TargetType::Auc24 :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);


        if (bOk) {
            // Calculate the AUC on 24h
            double auc24 = auc * 24.0 / intervalInHours;

            evaluateValue(auc24, _target, bOk, score, value);
        }
    } break;

    case TargetType::CumulativeAuc :
    {
        // TODO : We only take one compartment: to be checked
        std::vector<double> cumulativeAuc(1, 0.0);

        //Tucuxi::Core::CycleStatistics stats(cycleData, cumulativeAuc);
        for(std::size_t i = 0; i < _prediction.getTimes().size(); i++) {
            TimeOffsets times = _prediction.getTimes()[i];
            DateTime start = _intakeSeries[i].getEventTime();
            DateTime end = start + std::chrono::milliseconds(static_cast<int>(times.back()) * 1000);
            CycleData cycle(start, end, Unit("ug/l"));
            cycle.addData(times, _prediction.getValues()[i]);

            // The constructor accumulates in cumulativeAuc.
            CycleStatistics statisticsCalculator(cycle, cumulativeAuc);
        }

        evaluateValue(cumulativeAuc[0], _target, bOk, score, value);

    } break;

    case TargetType::Mean :
    {
        double mean = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::Mean);
        bOk = cycleStatistic.getValue(dateTime, mean);

        if (bOk) {
            evaluateValue(mean, _target, bOk, score, value);
        }
    } break;

    case TargetType::AucDividedByMic :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);


        if (bOk) {
            // Calculate the AUC
            double aucDividedByMic = auc / _target.m_mic;

            evaluateValue(aucDividedByMic, _target, bOk, score, value);
        }
    } break;

    case TargetType::Auc24DividedByMic :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the AUC on 24h
            double auc24 = auc * 24.0 / intervalInHours;
            double aucDividedByMic = auc24 / _target.m_mic;

            evaluateValue(aucDividedByMic, _target, bOk, score, value);
        }
    } break;

    case TargetType::AucOverMic :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the value between AUC and MIC
            double aucOverMic = auc - _target.m_mic;

            evaluateValue(aucOverMic, _target, bOk, score, value);
        }
    } break;

    case TargetType::Auc24OverMic :
    {
        double auc = -1.0;
        CycleStatistic cycleStatistic = statisticsCalculator.getStatistic(0, CycleStatisticType::AUC);
        bOk = cycleStatistic.getValue(dateTime, auc);

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        if (bOk) {
            // Calculate the value between AUC and MIC on 24h
            double auc24 = auc * 24.0 /intervalInHours;
            double auc24OverMic = auc24 - _target.m_mic;

            evaluateValue(auc24OverMic, _target, bOk, score, value);
        }
    } break;

    case TargetType::TimeOverMic :
    {
        return ComputingStatus::TargetEvaluationError;

        double intervalInHours = 0.0;
        CycleStatistic cycleStatisticForCycleInterval = statisticsCalculator.getStatistic(0, CycleStatisticType::CycleInterval);
        bOk &= cycleStatisticForCycleInterval.getValue(dateTime, intervalInHours);

        //intervalInHours vaut la dernièe valeur des TimeOffsets (47.5)

        if (bOk) {
            // Calculate the time when over mic
            TimeStruct timeStruct;
            Concentrations concentrations = cycle.getConcentrations()[0]; // [0] because only one concentrations (vector of concentration) in CycleData
            timeStruct.nbCompleteInterval = 0;
            for(int i = 0; i < int(concentrations.size()); i++){

                //Trigger up
                if ((concentrations[i] > _target.m_mic) && !timeStruct.timeFound[0]){

                    if(i > 1)
                    {
                        double ratio = concentrations[i] / concentrations[i - 1];
                        timeStruct.timeIntervals.push_back((cycle.getTimes()[0][i - 1]) * ratio);
                    }
                    else{
                        timeStruct.timeIntervals.push_back(cycle.getTimes()[0][i]);
                    }
                    timeStruct.timeFound[0] = true;
                    timeStruct.timeFound[1] = false;       // Can Trigger down again --> oscillation
                }

                //Trigger down if already up
                if(timeStruct.timeFound[0] && ((concentrations[i] < _target.m_mic) && !timeStruct.timeFound[1])){

                    double ratio = concentrations[i - 1] / concentrations[i];

                    timeStruct.timeIntervals[timeStruct.timeIntervals.size() - 1] = ((cycle.getTimes()[0][i - 1] * ratio) - timeStruct.timeIntervals.back());
                    timeStruct.timeFound[1] = true;
                    timeStruct.timeFound[0] = false;               // Can Trigger up again --> oscillation
                    timeStruct.nbCompleteInterval++;
                }

            }

            double timeOverMic = 0.0;

            //If no end value, use intervalInHours
            if ((timeStruct.timeFound[0] && !timeStruct.timeFound[1]) && (timeStruct.nbCompleteInterval == 0)){
                timeOverMic = intervalInHours - timeStruct.timeIntervals[0];
            }

            for(int j = 0; j < int(timeStruct.timeIntervals.size()); j++){

                // Last interval without end value
                if(timeStruct.nbCompleteInterval != int(timeStruct.timeIntervals.size())){

                    if(j == (int(timeStruct.timeIntervals.size()) - 1)){
                        timeOverMic += intervalInHours - timeStruct.timeIntervals[j];
                    }
                    else{
                        timeOverMic += timeStruct.timeIntervals[j];
                    }
                }
                else{
                    timeOverMic += timeStruct.timeIntervals[j];
                }
            }
            evaluateValue(timeOverMic, _target, bOk, score, value);

        }

    } break;

    case TargetType::PeakDividedByMic :
    {
        return ComputingStatus::TargetEvaluationError;
    } break;

    case TargetType::UnknownTarget :
    {
        return ComputingStatus::TargetEvaluationError;

    } break;


    }

    if (!bOk) {
        return ComputingStatus::InvalidCandidate;
    }

    // We build the result, as there was no error
    _result = TargetEvaluationResult(_target.m_targetType, score, translateToUnit(value, _target.m_unit, _target.m_finalUnit), _target.m_finalUnit);
    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
