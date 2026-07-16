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

namespace Tucuxi {
namespace Core {

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

    auto trait = std::make_unique<ComputingTraitPercentiles>(
            "rangePercentiles", _dosageStart, _dosageEnd, ranks, m_options.m_pointPerHour, option);

    ComputingRequest request("rangePercentiles", _drugModel, _drugTreatment, std::move(trait));
    auto response = std::make_unique<ComputingResponse>("rangePercentiles");

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

EtodaPointResult ComputingEtoda::evaluatePair(
        double _measuredConc, double _trueConc, const Tucuxi::Common::DateTime& _sampleDate)
{
}

int ComputingEtoda::classifyMetric(double _metricValue) const {}

bool ComputingEtoda::extractMetric(
        const Tucuxi::Core::SinglePredictionData& _predData, const std::string& _cycleUnit, double& _metricValue) const
{
}

ComputingStatus ComputingEtoda::compute(
        const ComputingTraitEtoda* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
}

ComputingStatus ComputingEtoda::compute(
        const ComputingTraitAdjustment* _traits,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response)
{
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
    std::vector<EtodaHourResult> results;

    for (double hour : m_options.m_samplingHours) {
        EtodaHourResult hourResult;
        hourResult.m_samplingHour = hour;
        DateTime sampleDate = _traits->getStart() + Duration(std::chrono::hours(static_cast<int>(hour)));

        // Loop over the full (measuredConc × trueConc) grid
        size_t total = concList.size() * concList.size();
        size_t current = 0;

        for (double measured : concList) {
            for (double trueConc : concList) {
                ++current;
                if (current % 10 == 0) {
                    std::cout << "\r  Hour " << hour << "h  [" << current << "/" << total << "]" << std::flush;
                }

                EtodaPointResult pt = evaluatePair(measured, trueConc, sampleDate);
                pt.m_samplingHour = hour;
                hourResult.m_points.push_back(pt);
            }
        }
        std::cout << "\n";

        results.push_back(std::move(hourResult));
    }
}

} // namespace Core
} // namespace Tucuxi
