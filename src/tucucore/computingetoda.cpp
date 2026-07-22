/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "computingetoda.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/computingservice/computingrequest.h"
#include "tucucore/overloadevaluator.h"

namespace Tucuxi {
namespace Core {

Tucuxi::Common::Duration ComputingEtoda::extractDefaultInterval(const DrugModel& _drugModel) const
{
    // ── Default formulation and route ─────────────────────────────────────────
    const FormulationAndRoutes& fars = _drugModel.getFormulationAndRoutes();
    const FullFormulationAndRoute* defaultFar = fars.getDefault();

    if (defaultFar == nullptr) {
        throw std::runtime_error("Drug model has no default formulation and route");
    }

    // ── Default dose interval ─────────────────────────────────────────────────
    const ValidDurations* validIntervals = defaultFar->getValidIntervals();
    if (validIntervals == nullptr) {
        throw std::runtime_error("Drug model has no valid intervals");
    }
    double intervalValue = validIntervals->getDefaultValue();
    std::string intervalUnit = validIntervals->getUnit().toString();
    double intervalSeconds = 0.0;
    if (intervalUnit == "min" || intervalUnit == "minutes") {
        intervalSeconds = intervalValue * 60.0;
    }
    else {
        // Assume hours
        intervalSeconds = intervalValue * 3600.0;
    }

    return Duration(std::chrono::seconds(static_cast<long long>(intervalSeconds)));
}

DrugTreatment ComputingEtoda::cloneDrugTreatment(const DrugTreatment& _drugTreatment)
{
    DrugTreatment clonedTreatment;

    // Clone covariates
    for (const auto& covariate : _drugTreatment.getCovariates()) {
        clonedTreatment.addCovariate(std::make_unique<PatientCovariate>(*covariate));
    }

    // Clone dosage time ranges
    for (const auto& timeRange : _drugTreatment.getDosageHistory().getDosageTimeRanges()) {
        clonedTreatment.addDosageTimeRange(std::make_unique<DosageTimeRange>(*timeRange));
    }

    // Clone targets
    for (const auto& target : _drugTreatment.getTargets()) {
        clonedTreatment.addTarget(std::make_unique<Target>(*target));
    }

    // Clone samples
    for (const auto& sample : _drugTreatment.getSamples()) {
        clonedTreatment.addSample(std::make_unique<Sample>(*sample));
    }

    return clonedTreatment;
}

int ComputingEtoda::computeConcentrationRange(
        const Tucuxi::Common::DateTime _dosageStart,
        const Tucuxi::Common::DateTime _dosageEnd,
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment,
        double& _minConc,
        double& _maxConc)
{
    _minConc = std::numeric_limits<double>::max();
    _maxConc = std::numeric_limits<double>::min();

    ComputingOption option(
            PredictionParameterType::Apriori,
            CompartmentsOption::AllActiveMoieties,
            RetrieveStatisticsOption::DoNotRetrieveStatistics,
            RetrieveParametersOption::DoNotRetrieveParameters,
            RetrieveCovariatesOption::DoNotRetrieveCovariates,
            ForceUgPerLiterOption::Force);

    PercentileRanks ranks;
    for (auto i = m_options.m_percentileRanks[0]; i <= m_options.m_percentileRanks[1]; i += 1.0) {
        ranks.push_back(i);
    }

    DateTime rangeStart = _dosageEnd - extractDefaultInterval(_drugModel);

    auto trait = std::make_unique<ComputingTraitPercentiles>(
            "rangePercentiles", rangeStart, _dosageEnd, ranks, m_options.m_pointPerHour, option);

    ComputingRequest request("rangePercentiles", _drugModel, _drugTreatment, std::move(trait));
    auto response = std::make_unique<ComputingResponse>("rangePercentiles");

    SingleOverloadEvaluator::getInstance()->setValues(100000, 5000, 10000);

    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
    if (component == nullptr) {
        return 1;
    }

    ComputingStatus status = component->compute(request, response);
    delete component;

    if (status != ComputingStatus::Ok) {
        return 1;
    }

    const auto* percData = dynamic_cast<const PercentilesData*>(response->getData());
    if (percData == nullptr) {
        return 1;
    }

    for (size_t ri = 0; ri < percData->getNbRanks(); ++ri) {
        for (const CycleData& cycle : percData->getPercentileData(ri)) {
            const auto concentrations = cycle.getConcentrations()[0];
            for (double c : concentrations) {
                if (c < _minConc) {
                    _minConc = c;
                }
                if (c > _maxConc) {
                    _maxConc = c;
                }
            }
        }
    }

    return 0;
}

std::unique_ptr<AdjustmentData> ComputingEtoda::findAdjustement(
        const Tucuxi::Common::DateTime _dosageStart,
        const Tucuxi::Common::DateTime _dosageEnd,
        const Tucuxi::Common::DateTime _adjustmentEnd,
        double _measuredConc,
        const Tucuxi::Common::DateTime& _sampleDate,
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment)
{
    // Create Drug Treatment with the measured concentration as a sample

    DrugTreatment treatmentWithSample = cloneDrugTreatment(_drugTreatment);

    AnalyteId analyteId(_drugModel.getDrugId());
    auto sample = std::make_unique<Sample>(_sampleDate, analyteId, _measuredConc, TucuUnit("ug/l"));
    treatmentWithSample.addSample(std::move(sample));

    // Create adjustment computing request

    ComputingOption adjOption(
            PredictionParameterType::Aposteriori,
            CompartmentsOption::AllActiveMoieties,
            RetrieveStatisticsOption::RetrieveStatistics,
            RetrieveParametersOption::DoNotRetrieveParameters,
            RetrieveCovariatesOption::DoNotRetrieveCovariates,
            ForceUgPerLiterOption::Force);

    auto adjTrait = std::make_unique<ComputingTraitAdjustment>(
            "adjustment",
            _dosageEnd,
            _adjustmentEnd,
            m_options.m_pointPerHour,
            adjOption,
            _dosageEnd,
            BestCandidatesOption::BestDosage,
            LoadingOption::NoLoadingDose,
            RestPeriodOption::NoRestPeriod,
            SteadyStateTargetOption::AtSteadyState,
            TargetExtractionOption::PopulationValues,
            FormulationAndRouteSelectionOption::LastFormulationAndRoute);

    ComputingRequest adjRequest("adjustment", _drugModel, treatmentWithSample, std::move(adjTrait));
    auto adjResponse = std::make_unique<ComputingResponse>("adjustment");

    auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());

    ComputingStatus adjStatus = component->compute(adjRequest, adjResponse);
    delete component;

    if (adjStatus != ComputingStatus::Ok) {
        return nullptr;
    }

    const auto* adjData = dynamic_cast<const AdjustmentData*>(adjResponse->getData());

    if (adjData == nullptr) {
        return nullptr;
    }

    return std::make_unique<AdjustmentData>(*adjData);
}

std::vector<EtodaPointResult> ComputingEtoda::evaluateAdjustment(
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment,
        std::unique_ptr<AdjustmentData>& _adjustmentData,
        double _measuredConc,
        double _sampleHour,
        const Tucuxi::Common::DateTime& _sampleDate,
        const Tucuxi::Common::DateTime _dosageStart,
        const Tucuxi::Common::DateTime _dosageEnd,
        const Tucuxi::Common::DateTime _adjustmentEnd,
        const Tucuxi::Core::TimeOffsets _concList)
{
    std::vector<EtodaPointResult> results;

    for (double trueConc : _concList) {
        EtodaPointResult& pointResult = results.emplace_back();
        pointResult.m_measuredConc = _measuredConc;
        pointResult.m_trueConc = trueConc;
        pointResult.m_samplingHour = _sampleHour;

        if (_adjustmentData == nullptr || _adjustmentData->getAdjustments().empty()) {
            pointResult.m_adjustmentFound = false;
            continue;
        }

        pointResult.m_adjustmentFound = true;
        const DosageAdjustment& bestAdj = _adjustmentData->getAdjustments()[0];
        const DosageHistory& adjHistory = bestAdj.m_history;

        DrugTreatment treatmentWithSample = cloneDrugTreatment(_drugTreatment);

        for (const auto& timeRange : adjHistory.getDosageTimeRanges()) {
            treatmentWithSample.getModifiableDosageHistory().addTimeRange(*timeRange);
        }

        AnalyteId analyteId(_drugModel.getDrugId());
        auto sample = std::make_unique<Sample>(_sampleDate, analyteId, trueConc, TucuUnit("ug/l"));
        treatmentWithSample.addSample(std::move(sample));

        ComputingOption predOption(
                PredictionParameterType::Aposteriori,
                CompartmentsOption::AllActiveMoieties,
                RetrieveStatisticsOption::RetrieveStatistics,
                RetrieveParametersOption::DoNotRetrieveParameters,
                RetrieveCovariatesOption::DoNotRetrieveCovariates,
                ForceUgPerLiterOption::Force);

        auto predTrait = std::make_unique<ComputingTraitConcentration>(
                "prediction", _dosageStart, _adjustmentEnd, m_options.m_pointPerHour, predOption);

        ComputingRequest predRequest("prediction", _drugModel, treatmentWithSample, std::move(predTrait));
        auto predResponse = std::make_unique<ComputingResponse>("prediction");

        auto* component = dynamic_cast<IComputingService*>(ComputingComponent::createComponent());
        if (component == nullptr) {
            throw std::runtime_error("Failed to create ComputingComponent");
        }

        ComputingStatus predStatus = component->compute(predRequest, predResponse);
        delete component;

        if (predStatus != ComputingStatus::Ok) {
            pointResult.m_adjustmentFound = false;
            continue;
        }

        const auto* predData = dynamic_cast<const SinglePredictionData*>(predResponse->getData());
        if (predData == nullptr || predData->getData().empty()) {
            pointResult.m_adjustmentFound = false;
            continue;
        }

        std::string cycleUnit;
        if (!predData->getData().empty()) {
            cycleUnit = predData->getData().back().m_unit.toString();
        }

        double metricValue = 0.0;
        if (!extractMetric(_drugModel, *predData, cycleUnit, metricValue)) {
            pointResult.m_adjustmentFound = false;
            continue;
        }

        pointResult.m_metricValue = metricValue;
        pointResult.m_zoneLabel = classifyMetric(_drugModel, metricValue);
    }

    return results;
}

bool ComputingEtoda::extractMetric(
        const DrugModel& _drugModel,
        const Tucuxi::Core::SinglePredictionData& _predData,
        const std::string& _cycleUnit,
        double& _metricValue) const
{
    const ActiveMoieties& activeMoieties = _drugModel.getActiveMoieties();
    const ActiveMoiety& moiety = *activeMoieties[0];
    const auto& targetDefinitions = moiety.getTargetDefinitions();

    if (targetDefinitions.empty()) {
        return false;
    }

    const auto& target = targetDefinitions[0];
    const std::vector<CycleData>& cycles = _predData.getData();

    if (cycles.size() < 2) {
        return false; // Need at least 2 cycles to access the penultimate one
    }

    // Python uses [-2] (penultimate cycle) for most targets, [-1] for cumulativeAuc
    size_t cycleIndex = (target->getTargetType() == TargetType::CumulativeAuc) ? cycles.size() - 1 : cycles.size() - 2;

    const CycleData& cycle = cycles[cycleIndex];

    if (cycle.m_statistics.getStats().empty()) {
        return false;
    }

    // ── Map TargetType to CycleStatisticType ──────────────────────────────────
    CycleStatisticType statType = CycleStatisticType::Mean;
    bool useMic = false;
    double mic = target->getMic().getValue();

    switch (target->getTargetType()) {
    case TargetType::Residual:
        statType = CycleStatisticType::Residual;
        break;
    case TargetType::Peak:
        statType = CycleStatisticType::Peak;
        break;
    case TargetType::Mean:
        statType = CycleStatisticType::Mean;
        break;
    case TargetType::Auc:
        statType = CycleStatisticType::AUC;
        break;
    case TargetType::Auc24:
        statType = CycleStatisticType::AUC24;
        break;
    case TargetType::CumulativeAuc:
        statType = CycleStatisticType::CumulativeAuc;
        break;
    case TargetType::Auc24DividedByMic:
        statType = CycleStatisticType::AUC24;
        useMic = true;
        break;
    case TargetType::AucDividedByMic:
        statType = CycleStatisticType::AUC;
        useMic = true;
        break;
    case TargetType::ResidualDividedByMic:
        statType = CycleStatisticType::Residual;
        useMic = true;
        break;
    case TargetType::PeakDividedByMic:
        statType = CycleStatisticType::Peak;
        useMic = true;
        break;
    default:
        statType = CycleStatisticType::Residual; // fallback
        break;
    }

    // ── Get the statistic value ───────────────────────────────────────────────
    CycleStatistic stat = cycle.m_statistics.getStatistic(0, statType);
    if (stat.getNbValue() == 0) {
        return false;
    }

    DateTime dummy;
    double rawValue = 0.0;
    if (!stat.getValue(dummy, rawValue)) {
        return false;
    }

    _metricValue = rawValue;

    // ── MIC division ──────────────────────────────────────────────────────────
    if (useMic) {
        if (mic <= 0.0) {
            return false;
        }
        // Unit correction: if MIC is in mg/l and cycle conc is in ug/l → divide by 1000
        if (!target->getMicUnit().toString().empty() && !_cycleUnit.empty()
            && target->getMicUnit().toString().size() >= 2 && _cycleUnit.size() >= 2
            && target->getMicUnit().toString().substr(0, 2) == "mg" && _cycleUnit.substr(0, 2) == "ug") {
            mic *= 1000.0;
        }
        _metricValue /= mic;
    }

    // ── Unit correction for non-MIC targets ──────────────────────────────────
    if (!useMic) {
        std::string targetUnit = target->getUnit().toString();
        if (!targetUnit.empty() && !_cycleUnit.empty() && targetUnit.size() >= 2 && _cycleUnit.size() >= 2
            && targetUnit.substr(0, 2) == "mg" && _cycleUnit.substr(0, 2) == "ug") {
            _metricValue /= 1000.0;
        }
    }

    return true;
}

int ComputingEtoda::classifyMetric(const DrugModel& _drugModel, double _metricValue) const
{
    const ActiveMoieties& activeMoieties = _drugModel.getActiveMoieties();
    const ActiveMoiety& moiety = *activeMoieties[0];
    const auto& targetDefinitions = moiety.getTargetDefinitions();

    if (targetDefinitions.empty()) {
        return 0;
    }

    const auto& t = targetDefinitions[0];

    // threshold_range = [[0, inefficacy], [inefficacy, min], [min, max], [max, toxicity], [toxicity, inf]]
    // threshold_label = [-2, -1, 0, 1, 2]
    if (_metricValue >= 0.0 && _metricValue < t->getInefficacyAlarm().getValue()) {
        return -2;
    }
    if (_metricValue >= t->getInefficacyAlarm().getValue() && _metricValue < t->getCMin().getValue()) {
        return -1;
    }
    if (_metricValue >= t->getCMin().getValue() && _metricValue < t->getCMax().getValue()) {
        return 0;
    }
    if (_metricValue >= t->getCMax().getValue() && _metricValue < t->getToxicityAlarm().getValue()) {
        return 1;
    }
    if (_metricValue >= t->getToxicityAlarm().getValue()) {
        return 2;
    }

    return 0; // fallback (negative values not expected in PK metrics)
}

ComputingStatus ComputingEtoda::compute(
        const ComputingTraitEtoda* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
    SingleOverloadEvaluator::getInstance()->setValues(100000, 5000, 10000);

    const DrugModel& drugModel = _request.getDrugModel();
    const DrugTreatment& drugTreatment = _request.getDrugTreatment();

    // ── Determine concentration grid ─────────────────────────────────────────
    double minConc = 0.0;
    double maxConc = 0.0;
    if (computeConcentrationRange(_traits->getStart(), _traits->getEnd(), drugModel, drugTreatment, minConc, maxConc)
        != 0) {
        return ComputingStatus::ComputingComponentExceptionError;
    }

    // Build the concentration list (linspace equivalent)
    std::vector<double> concList(static_cast<size_t>(m_options.m_numConcentrationPoints));
    double step = (maxConc - minConc) / (m_options.m_numConcentrationPoints - 1);
    for (int i = 0; i < m_options.m_numConcentrationPoints; ++i) {
        concList[static_cast<size_t>(i)] = minConc + i * step;
    }

    // ── Loop over sampling hours ──────────────────────────────────────────────
    std::unique_ptr<EtodaData> results = std::make_unique<EtodaData>(_traits->getId());

    for (double hour : m_options.m_samplingHours) {
        EtodaHourResult hourResult;
        hourResult.m_samplingHour = hour;
        DateTime sampleDate = _traits->getSampleDate() + Duration(std::chrono::hours(static_cast<int>(hour)));

        // Loop over the full (measuredConc × trueConc) grid
        size_t total = concList.size() * concList.size();
        size_t current = 0;

        for (double measured : concList) {
            auto adjustmentData = findAdjustement(
                    _traits->getStart(),
                    _traits->getEnd(),
                    _traits->getAdjustmentEnd(),
                    measured,
                    sampleDate,
                    drugModel,
                    drugTreatment);

            std::vector<EtodaPointResult> pointResults = evaluateAdjustment(
                    drugModel,
                    drugTreatment,
                    adjustmentData,
                    measured,
                    hour,
                    sampleDate,
                    _traits->getStart(),
                    _traits->getEnd(),
                    _traits->getAdjustmentEnd(),
                    concList);

            hourResult.m_points.insert(hourResult.m_points.end(), pointResults.begin(), pointResults.end());
        }

        results->addEtodaHourResult(hourResult);
    }

    _response->addResponse(std::move(results));
    return ComputingStatus::Ok;
}

ComputingStatus ComputingEtoda::compute(
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
    return ComputingStatus::Undefined;
}

} // namespace Core
} // namespace Tucuxi
