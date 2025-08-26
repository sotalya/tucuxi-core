//@@license@@

#include <gtest/gtest.h>

#include "computingresponsecomparator.h"


using namespace Tucuxi::Core;

void compareSinglePoints(const SinglePointsData* _d1, const SinglePointsData* _d2)
{
    ASSERT_DOUBLE_EQ(_d1->getLogLikelihood(), _d2->getLogLikelihood());
    ASSERT_EQ(_d1->m_times.size(), _d2->m_times.size());
    ASSERT_EQ(_d1->m_concentrations.size(), _d2->m_concentrations.size());
    ASSERT_EQ(_d1->m_unit, _d2->m_unit);
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

void compareCycleData(const CycleData& _d1, const CycleData& _d2, bool _isPercentile = false)
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

void compareCycleDatas(const std::vector<CycleData>& _d1, const std::vector<CycleData>& _d2, bool _isPercentile = false)
{
    ASSERT_EQ(_d1.size(), _d2.size());
    for (size_t cycle = 0; cycle < _d1.size(); cycle++) {
        compareCycleData(_d1[cycle], _d2[cycle], _isPercentile);
    }
}

void compareCompartmentInfos(const std::vector<CompartmentInfo>& _d1, const std::vector<CompartmentInfo>& _d2)
{
    ASSERT_EQ(_d1.size(), _d2.size());
    for (size_t i = 0; i < _d1.size(); i++) {
        ASSERT_EQ(_d1[i].getId(), _d2[i].getId());
        ASSERT_EQ(_d1[i].getType(), _d2[i].getType());
    }
}

void compareSinglePrediction(const SinglePredictionData* _d1, const SinglePredictionData* _d2)
{
    ASSERT_DOUBLE_EQ(_d1->getLogLikelihood(), _d2->getLogLikelihood());
    compareCompartmentInfos(_d1->getCompartmentInfos(), _d2->getCompartmentInfos());
    compareCycleDatas(_d1->getData(), _d2->getData());
}


void compareLastingDose(const LastingDose& _d1, const LastingDose& _d2)
{
    ASSERT_EQ(_d1.getDose(), _d2.getDose());
    ASSERT_EQ(_d1.getTimeStep(), _d2.getTimeStep());
    ASSERT_EQ(_d1.getDoseUnit(), _d2.getDoseUnit());
    ASSERT_EQ(_d1.getInfusionTime(), _d2.getInfusionTime());
    ASSERT_EQ(_d1.getFormulationAndRouteList(), _d1.getFormulationAndRouteList());
}

void compareDosageLoop(const DosageLoop& _d1, const DosageLoop& _d2)
{
    auto d1 = _d1.getDosage();
    auto d2 = _d2.getDosage();
    if (dynamic_cast<const LastingDose*>(d1) != nullptr) {
        compareLastingDose(*dynamic_cast<const LastingDose*>(d1), *dynamic_cast<const LastingDose*>(d2));
    }
}

void compareDosageTimeRange(const DosageTimeRange& _d1, const DosageTimeRange& _d2)
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

void compareDosageHistory(const DosageHistory& _d1, const DosageHistory& _d2)
{
    ASSERT_EQ(_d1.getDosageTimeRanges().size(), _d2.getDosageTimeRanges().size());
    for (size_t i = 0; i < _d1.getDosageTimeRanges().size(); i++) {
        compareDosageTimeRange(*_d1.getDosageTimeRanges()[i], *_d2.getDosageTimeRanges()[i]);
    }
}

void compareDosageAdjustment(const DosageAdjustment& _d1, const DosageAdjustment& _d2)
{
    //ASSERT_DOUBLE_EQ(_d1.getGlobalScore(), _d2.getGlobalScore());
    compareCycleDatas(_d1.getData(), _d2.getData());
    compareCompartmentInfos(_d1.getCompartmentInfos(), _d2.getCompartmentInfos());
    compareDosageHistory(_d1.getDosageHistory(), _d2.getDosageHistory());
}

void compareAdjustment(const AdjustmentData* _d1, const AdjustmentData* _d2)
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
}

void comparePercentiles(const PercentilesData* _d1, const PercentilesData* _d2)
{
    ASSERT_EQ(_d1->getId(), _d2->getId());
    ASSERT_EQ(_d1->getNbRanks(), _d2->getNbRanks());
    ASSERT_EQ(_d1->getNbPointsPerHour(), _d2->getNbPointsPerHour());
    size_t nbRanks = _d1->getNbRanks();
    for (size_t i = 0; i < nbRanks; i++) {
        compareCycleDatas(_d1->getPercentileData(i), _d2->getPercentileData(i), true);
    }
}

void ComputingResponseComparator::compare(Tucuxi::Core::ComputingStatus _s1, Tucuxi::Core::ComputingStatus _s2)
{
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

    ASSERT_EQ(_r1.getComputingStatus(), _r2.getComputingStatus());
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
    if (!isSomething) {
        // If something went wrong with the computation, the data results will be nullptr
        ASSERT_EQ(_r1.getData(), nullptr);
        ASSERT_EQ(_r2.getData(), nullptr);
    }
}
