//@@license@@

#include <gtest/gtest.h>

#include "computingresponsecomparator.h"


using namespace Tucuxi::Core;


void ComputingResponseComparator::compareCompartmentInfos(
        const std::vector<CompartmentInfo>& _d1, const std::vector<CompartmentInfo>& _d2)
{
    ASSERT_EQ(_d1.size(), _d2.size());
    for (size_t i = 0; i < _d1.size(); i++) {
        ASSERT_EQ(_d1[i].getId(), _d2[i].getId());
        ASSERT_EQ(_d1[i].getType(), _d2[i].getType());
    }
}

void ComputingResponseComparator::compareGof(const GofData* _d1, const GofData* _d2)
{
    if (_d1 == nullptr && _d2 == nullptr) {
        return;
    }
    ASSERT_NE(_d1, nullptr);
    ASSERT_NE(_d2, nullptr);
    ASSERT_DOUBLE_EQ(_d1->getMae(), _d2->getMae());
    ASSERT_DOUBLE_EQ(_d1->getMape(), _d2->getMape());
    ASSERT_DOUBLE_EQ(_d1->getMse(), _d2->getMse());
    ASSERT_DOUBLE_EQ(_d1->getRmse(), _d2->getRmse());
    ASSERT_DOUBLE_EQ(_d1->getRmsle(), _d2->getRmsle());
    ASSERT_DOUBLE_EQ(_d1->getRrmse(), _d2->getRrmse());
    ASSERT_DOUBLE_EQ(_d1->getRSquared(), _d2->getRSquared());
    ASSERT_DOUBLE_EQ(_d1->getMeanPredictionError(), _d2->getMeanPredictionError());
    ASSERT_DOUBLE_EQ(_d1->getMeanAbsolutePredictionError(), _d2->getMeanAbsolutePredictionError());
    ASSERT_EQ(_d1->getUnit(), _d2->getUnit());
    ASSERT_EQ(_d1->getPredErrors().size(), _d2->getPredErrors().size());
    for (size_t i = 0; i < _d1->getPredErrors().size(); i++) {
        auto e1 = _d1->getPredErrors()[i];
        auto e2 = _d2->getPredErrors()[i];
        ASSERT_DOUBLE_EQ(e1.getMeasure(), e2.getMeasure());
        ASSERT_DOUBLE_EQ(e1.getPrediction(), e2.getPrediction());
        ASSERT_DOUBLE_EQ(e1.getPredictionError(), e2.getPredictionError());
        ASSERT_DOUBLE_EQ(e1.getAbsPredErrorPct(), e2.getAbsPredErrorPct());
    }
}

void ComputingResponseComparator::compareSinglePoints(const SinglePointsData* _d1, const SinglePointsData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    ASSERT_DOUBLE_EQ(_d1->getLogLikelihood(), _d2->getLogLikelihood());
    ASSERT_EQ(_d1->m_times.size(), _d2->m_times.size());
    ASSERT_EQ(_d1->m_concentrations.size(), _d2->m_concentrations.size());
    ASSERT_EQ(_d1->m_unit, _d2->m_unit);
    compareCompartmentInfos(_d1->getCompartmentInfos(), _d2->getCompartmentInfos());
    compareGof(_d1->getGof(), _d2->getGof());
    for (size_t i = 0; i < _d1->m_times.size(); i++) {
        ASSERT_EQ(_d1->m_times[i], _d2->m_times[i]);
    }
    for (size_t cIndex = 0; cIndex < _d1->m_concentrations.size(); cIndex++) {
        auto c1 = _d1->m_concentrations[cIndex];
        auto c2 = _d2->m_concentrations[cIndex];
        ASSERT_EQ(c1.size(), c2.size());
        for (size_t c = 0; c < c1.size(); c++) {
            ASSERT_DOUBLE_EQ(c1[c], c2[c]);
        }
    }
}

void ComputingResponseComparator::compareCycleData(const CycleData& _d1, const CycleData& _d2, bool _isPercentile)
{
    ASSERT_EQ(_d1.m_unit, _d2.m_unit);
    ASSERT_EQ(_d1.m_start, _d2.m_start);
    ASSERT_EQ(_d1.m_end, _d2.m_end);
    ASSERT_EQ(_d1.getTimes().size(), _d2.getTimes().size());
    for (size_t c = 0; c < _d1.getTimes().size(); c++) {
        for (size_t cc = 0; cc < _d1.getTimes()[c].size(); cc++) {
            ASSERT_DOUBLE_EQ(_d1.getTimes()[c][cc], _d2.getTimes()[c][cc]);
        }
    }
    for (size_t cIndex = 0; cIndex < _d1.m_concentrations.size(); cIndex++) {
        auto c1 = _d1.m_concentrations[cIndex];
        auto c2 = _d2.m_concentrations[cIndex];
        ASSERT_EQ(c1.size(), c2.size());
        for (size_t c = 0; c < c1.size(); c++) {
            if (_isPercentile) {
                // TODO : Find a way to have a fair comparison
                // ASSERT_TRUE(std::abs(c1[c] - c2[c]) < 300); double_fuzzy_compare()
            }
            else {
                ASSERT_DOUBLE_EQ(c1[c], c2[c]);
            }
        }
    }
    // Covariates
    ASSERT_EQ(_d1.m_covariates.size(), _d2.m_covariates.size());
    for (size_t i = 0; i < _d1.m_covariates.size(); i++) {
        auto cov1 = _d1.m_covariates[i];
        auto cov2 = _d2.m_covariates[i];
        ASSERT_EQ(cov1.m_covariateId, cov2.m_covariateId);
        ASSERT_EQ(cov1.m_value, cov2.m_value);
    }
    // Parameters
    ASSERT_EQ(_d1.m_parameters.size(), _d2.m_parameters.size());
    for (size_t i = 0; i < _d1.m_parameters.size(); i++) {
        auto p1 = _d1.m_parameters[i];
        auto p2 = _d2.m_parameters[i];
        ASSERT_EQ(p1.m_parameterId, p2.m_parameterId);
        ASSERT_EQ(p1.m_value, p2.m_value);
    }
    // Statistics
    ASSERT_EQ(_d1.m_statistics.getStats().size(), _d2.m_statistics.getStats().size());
    for (size_t i = 0; i < _d1.m_statistics.getStats().size(); i++) {
        auto s1 = _d1.m_statistics.getStats()[i];
        auto s2 = _d2.m_statistics.getStats()[i];
        ASSERT_EQ(s1.size(), s2.size());
        for (size_t j = 0; j < s1.size(); j++) {
            auto st1 = s1[j];
            auto st2 = s2[j];
            ASSERT_EQ(st1.getCycleStartDate(), st2.getCycleStartDate());
            ASSERT_EQ(st1.getNbValue(), st2.getNbValue());
            ASSERT_EQ(st1.getRawData().size(), st2.getRawData().size());
            for (size_t rd = 0; rd < st1.getRawData().size(); rd++) {
                auto rd1 = st1.getRawData()[rd];
                auto rd2 = st2.getRawData()[rd];
                ASSERT_EQ(rd1.m_offset, rd2.m_offset);
                if (_isPercentile) {
                    // TODO : Find a way to have a fair comparison
                    // ASSERT_TRUE(std::abs(rd1.m_value - rd2.m_value) < 300); double_fuzzy_compare()
                }
                else {
                    ASSERT_EQ(rd1.m_value, rd2.m_value);
                }
            }
        }
    }
}

void ComputingResponseComparator::compareCycleDatas(
        const std::vector<CycleData>& _d1, const std::vector<CycleData>& _d2, bool _isPercentile)
{
    ASSERT_EQ(_d1.size(), _d2.size());
    for (size_t cycle = 0; cycle < _d1.size(); cycle++) {
        compareCycleData(_d1[cycle], _d2[cycle], _isPercentile);
    }
}

void ComputingResponseComparator::compareSinglePrediction(
        const SinglePredictionData* _d1, const SinglePredictionData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    ASSERT_DOUBLE_EQ(_d1->getLogLikelihood(), _d2->getLogLikelihood());
    compareCompartmentInfos(_d1->getCompartmentInfos(), _d2->getCompartmentInfos());
    compareCycleDatas(_d1->getData(), _d2->getData());
    compareGof(_d1->getGof(), _d2->getGof());
}


void ComputingResponseComparator::compareLastingDose(const LastingDose& _d1, const LastingDose& _d2)
{
    ASSERT_EQ(_d1.getDose(), _d2.getDose());
    ASSERT_EQ(_d1.getTimeStep(), _d2.getTimeStep());
    ASSERT_EQ(_d1.getDoseUnit(), _d2.getDoseUnit());
    ASSERT_EQ(_d1.getInfusionTime(), _d2.getInfusionTime());
    ASSERT_EQ(_d1.getFormulationAndRouteList(), _d2.getFormulationAndRouteList());
}

void ComputingResponseComparator::compareDosageLoop(const DosageLoop& _d1, const DosageLoop& _d2)
{
    auto d1 = _d1.getDosage();
    auto d2 = _d2.getDosage();
    if (dynamic_cast<const LastingDose*>(d1) != nullptr) {
        compareLastingDose(*dynamic_cast<const LastingDose*>(d1), *dynamic_cast<const LastingDose*>(d2));
    }
}

void ComputingResponseComparator::compareDosageTimeRange(const DosageTimeRange& _d1, const DosageTimeRange& _d2)
{
    ASSERT_EQ(_d1.getStartDate(), _d2.getStartDate());
    ASSERT_EQ(_d1.getEndDate(), _d2.getEndDate());
    // TODO Continue
    auto d1 = _d1.getDosage();
    auto d2 = _d2.getDosage();
    if (dynamic_cast<const DosageLoop*>(d1) != nullptr) {
        compareDosageLoop(*dynamic_cast<const DosageLoop*>(d1), *dynamic_cast<const DosageLoop*>(d2));
    }
}

void ComputingResponseComparator::compareDosageHistory(const DosageHistory& _d1, const DosageHistory& _d2)
{
    ASSERT_EQ(_d1.getDosageTimeRanges().size(), _d2.getDosageTimeRanges().size());
    for (size_t i = 0; i < _d1.getDosageTimeRanges().size(); i++) {
        compareDosageTimeRange(*_d1.getDosageTimeRanges()[i], *_d2.getDosageTimeRanges()[i]);
    }
}

void ComputingResponseComparator::compareTargetEvaluationResult(
        const TargetEvaluationResult& _d1, const TargetEvaluationResult& _d2)
{
    ASSERT_EQ(_d1.getTargetType(), _d2.getTargetType());
    if (m_compareTargetScoreValue) {
        ASSERT_DOUBLE_EQ(_d1.getScore(), _d2.getScore());
        ASSERT_DOUBLE_EQ(_d1.getValue(), _d2.getValue());
    }
    ASSERT_EQ(_d1.getUnit(), _d2.getUnit());
    ASSERT_EQ(_d1.getTarget().getActiveMoietyId(), _d2.getTarget().getActiveMoietyId());
    ASSERT_DOUBLE_EQ(_d1.getTarget().getValueMin(), _d2.getTarget().getValueMin());
    ASSERT_DOUBLE_EQ(_d1.getTarget().getValueMax(), _d2.getTarget().getValueMax());
    ASSERT_DOUBLE_EQ(_d1.getTarget().getValueBest(), _d2.getTarget().getValueBest());
    ASSERT_DOUBLE_EQ(_d1.getTarget().getInefficacyAlarm(), _d2.getTarget().getInefficacyAlarm());
    ASSERT_DOUBLE_EQ(_d1.getTarget().getToxicityAlarm(), _d2.getTarget().getToxicityAlarm());
}

void ComputingResponseComparator::compareDosageAdjustment(const DosageAdjustment& _d1, const DosageAdjustment& _d2)
{
    //ASSERT_DOUBLE_EQ(_d1.getGlobalScore(), _d2.getGlobalScore());
    compareCycleDatas(_d1.getData(), _d2.getData());
    compareCompartmentInfos(_d1.getCompartmentInfos(), _d2.getCompartmentInfos());
    compareDosageHistory(_d1.getDosageHistory(), _d2.getDosageHistory());
    ASSERT_EQ(_d1.m_targetsEvaluation.size(), _d2.m_targetsEvaluation.size());
    for (size_t i = 0; i < _d1.m_targetsEvaluation.size(); i++) {
        compareTargetEvaluationResult(_d1.m_targetsEvaluation[i], _d2.m_targetsEvaluation[i]);
    }
    ASSERT_EQ(_d1.getEtodaData().has_value(), _d2.getEtodaData().has_value());
    const auto& v1 = _d1.getEtodaData();
    const auto& v2 = _d2.getEtodaData();
    if (v1.has_value() && v2.has_value()) {
        compareEtoda(&v1.value(), &v2.value());
    }
}

void ComputingResponseComparator::compareAdjustment(const AdjustmentData* _d1, const AdjustmentData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    compareCompartmentInfos(_d1->getCompartmentInfos(), _d2->getCompartmentInfos());
    ASSERT_DOUBLE_EQ(_d1->getLogLikelihood(), _d2->getLogLikelihood());
    compareCycleDatas(_d1->getData(), _d2->getData());
    ASSERT_EQ(_d1->getAdjustments().size(), _d2->getAdjustments().size());
    for (size_t i = 0; i < _d1->getAdjustments().size(); i++) {
        compareDosageAdjustment(_d1->getAdjustments()[i], _d2->getAdjustments()[i]);
    }
    compareDosageAdjustment(_d1->getCurrentDosageWithScore(), _d2->getCurrentDosageWithScore());
    ASSERT_EQ(_d1->isCurrentInRange(), _d2->isCurrentInRange());
    compareGof(_d1->getGof(), _d2->getGof());
}

void ComputingResponseComparator::comparePercentiles(const PercentilesData* _d1, const PercentilesData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    ASSERT_EQ(_d1->getNbRanks(), _d2->getNbRanks());
    ASSERT_EQ(_d1->getNbPointsPerHour(), _d2->getNbPointsPerHour());
    ASSERT_EQ(_d1->getRanks(), _d2->getRanks());
    compareCompartmentInfos(_d1->getCompartmentInfos(), _d2->getCompartmentInfos());
    size_t nbRanks = _d1->getNbRanks();
    for (size_t i = 0; i < nbRanks; i++) {
        compareCycleDatas(_d1->getPercentileData(i), _d2->getPercentileData(i), true);
    }
}
void ComputingResponseComparator::compareEtoda(const Tucuxi::Core::EtodaData* _d1, const Tucuxi::Core::EtodaData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    ASSERT_EQ(_d1->getEtodaResults().size(), _d2->getEtodaResults().size());
    for (size_t i = 0; i < _d1->getEtodaResults().size(); i++) {
        const auto& v1 = _d1->getEtodaResults().at(i);
        const auto& v2 = _d2->getEtodaResults().at(i);
        ASSERT_EQ(v1.m_nbPointsMeasured, v2.m_nbPointsMeasured);
        ASSERT_EQ(v1.m_nbPointsTrue, v2.m_nbPointsTrue);
        ASSERT_EQ(v1.m_points.size(), v2.m_points.size());
        for (size_t j = 0; j < v1.m_points.size(); j++) {
            const auto& p1 = v1.m_points[j];
            const auto& p2 = v2.m_points[j];
            ASSERT_EQ(p1.m_adjustmentFound, p2.m_adjustmentFound);
            ASSERT_EQ(p1.m_measuredConc, p2.m_measuredConc);
            ASSERT_EQ(p1.m_trueConc, p2.m_trueConc);
            ASSERT_EQ(p1.m_metricValue, p2.m_metricValue);
            ASSERT_EQ(p1.m_samplingHour, p2.m_samplingHour);
            ASSERT_EQ(p1.m_zoneLabel, p2.m_zoneLabel);
        }
        ASSERT_EQ(v1.m_samplingHour, v2.m_samplingHour);
    }
    compareGof(_d1->getGof(), _d2->getGof());
}


void ComputingResponseComparator::compare(Tucuxi::Core::ComputingStatus _s1, Tucuxi::Core::ComputingStatus _s2)
{
    if ((_s1 == ComputingStatus::ComputingComponentExceptionError)
        && (_s2 == ComputingStatus::MultiComputingComponentExceptionError)) {
        return;
    }
    if ((_s1 == ComputingStatus::MultiComputingComponentExceptionError)
        && (_s2 == ComputingStatus::ComputingComponentExceptionError)) {
        return;
    }
    ASSERT_EQ(_s1, _s2);
}

void ComputingResponseComparator::compare(ComputingResponse& _r1, ComputingResponse& _r2)
{
#ifdef TUCU_COMPARE_TIME
    auto t1 = _r1.getComputingTimeInSeconds().count();
    auto t2 = _r2.getComputingTimeInSeconds().count();
    std::cout << "Time simple (ms) : " << ( t1 )*1000.0 << " . Time Multi (ms) : " << t2 * 1000.0 << '\n';
    std::cout << "Time diff (ms) : " << (t2 - t1) * 1000.0 << '\n';
    std::cout << "Time rel diff (%) : " << (t2 - t1) / t1 * 100.0 << '\n';
#endif // TUCU_COMPARE_TIME

    compare(_r1.getComputingStatus(), _r2.getComputingStatus());
    auto rawD1 = _r1.getData();
    auto rawD2 = _r2.getData();

    bool isSomething = false;
    if (dynamic_cast<const AdjustmentData*>(rawD1) != nullptr) {
        auto d1 = dynamic_cast<const AdjustmentData*>(rawD1);
        auto d2 = dynamic_cast<const AdjustmentData*>(rawD2);
        compareAdjustment(d1, d2);
        isSomething = true;
    }
    if (dynamic_cast<const SinglePointsData*>(rawD1) != nullptr) {
        auto d1 = dynamic_cast<const SinglePointsData*>(rawD1);
        auto d2 = dynamic_cast<const SinglePointsData*>(rawD2);
        compareSinglePoints(d1, d2);
        isSomething = true;
    }
    if (dynamic_cast<const SinglePredictionData*>(rawD1) != nullptr) {
        auto d1 = dynamic_cast<const SinglePredictionData*>(rawD1);
        auto d2 = dynamic_cast<const SinglePredictionData*>(rawD2);
        compareSinglePrediction(d1, d2);
        isSomething = true;
    }
    if (dynamic_cast<const PercentilesData*>(rawD1) != nullptr) {
        auto d1 = dynamic_cast<const PercentilesData*>(rawD1);
        auto d2 = dynamic_cast<const PercentilesData*>(rawD2);
        comparePercentiles(d1, d2);
        isSomething = true;
    }
    if (dynamic_cast<const EtodaData*>(rawD1) != nullptr) {
        auto d1 = dynamic_cast<const EtodaData*>(rawD1);
        auto d2 = dynamic_cast<const EtodaData*>(rawD2);
        compareEtoda(d1, d2);
        isSomething = true;
    }
    if (!isSomething) {
        // If something went wrong with the computation, the data results will be nullptr
        ASSERT_EQ(_r1.getData(), nullptr);
        ASSERT_EQ(_r2.getData(), nullptr);
    }
}
