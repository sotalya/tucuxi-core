//@@license@@

#ifndef COMPUTINGRESPONSECOMPARATOR_H
#define COMPUTINGRESPONSECOMPARATOR_H

#include <vector>

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/dosage.h"

class ComputingResponseComparator
{
public:
    void compare(Tucuxi::Core::ComputingStatus _s1, Tucuxi::Core::ComputingStatus _s2);

    void compare(Tucuxi::Core::ComputingResponse& _r1, Tucuxi::Core::ComputingResponse& _r2);

    void setCompareTargetScoreValue(bool _compareTargetScoreValue)
    {
        m_compareTargetScoreValue = _compareTargetScoreValue;
    }

private:
    void compareCompartmentInfos(
            const std::vector<Tucuxi::Core::CompartmentInfo>& _d1,
            const std::vector<Tucuxi::Core::CompartmentInfo>& _d2);
    void compareGof(const Tucuxi::Core::GofData* _d1, const Tucuxi::Core::GofData* _d2);
    void compareSinglePoints(const Tucuxi::Core::SinglePointsData* _d1, const Tucuxi::Core::SinglePointsData* _d2);
    void compareCycleData(
            const Tucuxi::Core::CycleData& _d1, const Tucuxi::Core::CycleData& _d2, bool _isPercentile = false);
    void compareCycleDatas(
            const std::vector<Tucuxi::Core::CycleData>& _d1,
            const std::vector<Tucuxi::Core::CycleData>& _d2,
            bool _isPercentile = false);
    void compareSinglePrediction(
            const Tucuxi::Core::SinglePredictionData* _d1, const Tucuxi::Core::SinglePredictionData* _d2);
    void compareLastingDose(const Tucuxi::Core::LastingDose& _d1, const Tucuxi::Core::LastingDose& _d2);
    void compareDosageLoop(const Tucuxi::Core::DosageLoop& _d1, const Tucuxi::Core::DosageLoop& _d2);
    void compareDosageTimeRange(const Tucuxi::Core::DosageTimeRange& _d1, const Tucuxi::Core::DosageTimeRange& _d2);
    void compareDosageHistory(const Tucuxi::Core::DosageHistory& _d1, const Tucuxi::Core::DosageHistory& _d2);
    void compareTargetEvaluationResult(
            const Tucuxi::Core::TargetEvaluationResult& _d1, const Tucuxi::Core::TargetEvaluationResult& _d2);
    void compareDosageAdjustment(const Tucuxi::Core::DosageAdjustment& _d1, const Tucuxi::Core::DosageAdjustment& _d2);
    void compareAdjustment(const Tucuxi::Core::AdjustmentData* _d1, const Tucuxi::Core::AdjustmentData* _d2);
    void comparePercentiles(const Tucuxi::Core::PercentilesData* _d1, const Tucuxi::Core::PercentilesData* _d2);
    void compareEtoda(const Tucuxi::Core::EtodaData* _d1, const Tucuxi::Core::EtodaData* _d2);

    bool m_compareTargetScoreValue{true};
};

#endif // COMPUTINGRESPONSECOMPARATOR_H
