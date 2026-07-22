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


#ifndef TUCUXI_CORE_COMPUTINGETODA_H
#define TUCUXI_CORE_COMPUTINGETODA_H

#include <vector>

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/drugtreatment/drugtreatment.h"

namespace Tucuxi {
namespace Core {

struct EtodaOptions
{
    std::vector<double> m_samplingHours{0.0, 2.0};
    int m_numConcentrationPoints{10};
    std::vector<double> m_percentileRanks{1.0, 99.0};
    double m_pointPerHour{20.0};
};

class ComputingEtoda
{
public:
    ComputingEtoda(const EtodaOptions& _options) : m_options(_options) {}

    [[nodiscard]] ComputingStatus compute(
            const ComputingTraitEtoda* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    [[nodiscard]] ComputingStatus compute(
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

private:
    [[nodiscard]] DrugTreatment cloneDrugTreatment(const DrugTreatment& _drugTreatment);

    [[nodiscard]] int computeConcentrationRange(
            const Tucuxi::Common::DateTime _dosageStart,
            const Tucuxi::Common::DateTime _dosageEnd,
            const DrugModel& _drugModel,
            const DrugTreatment& _drugTreatment,
            double& _minConc,
            double& _maxConc);

    [[nodiscard]] std::vector<EtodaPointResult> evaluateAdjustment(
            const DrugModel& _drugModel,
            const DrugTreatment& _drugTreatment,
            std::unique_ptr<AdjustmentData>& _adjustmentData,
            double _measuredConc,
            double _sampleHour,
            const Tucuxi::Common::DateTime& _sampleDate,
            const Tucuxi::Common::DateTime _dosageStart,
            const Tucuxi::Common::DateTime _dosageEnd,
            const Tucuxi::Common::DateTime _adjustmentEnd,
            const Tucuxi::Core::TimeOffsets _concList);

    [[nodiscard]] std::unique_ptr<AdjustmentData> findAdjustement(
            const Tucuxi::Common::DateTime _dosageStart,
            const Tucuxi::Common::DateTime _dosageEnd,
            const Tucuxi::Common::DateTime _adjustmentEnd,
            double _measuredConc,
            const Tucuxi::Common::DateTime& _sampleDate,
            const DrugModel& _drugModel,
            const DrugTreatment& _drugTreatment);

    [[nodiscard]] int classifyMetric(const DrugModel& _drugModel, double _metricValue) const;

    [[nodiscard]] bool extractMetric(
            const DrugModel& _drugModel,
            const Tucuxi::Core::SinglePredictionData& _predData,
            const std::string& _cycleUnit,
            double& _metricValue) const;

    [[nodiscard]] Tucuxi::Common::Duration extractDefaultInterval(const DrugModel& _drugModel) const;

    EtodaOptions m_options;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGETODA_H