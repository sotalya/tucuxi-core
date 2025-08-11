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


#include <utility>

#include "computingtrait.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/multicomputingcomponent.h"

namespace Tucuxi {
namespace Core {


ComputingOption::ComputingOption(
        PredictionParameterType _parameterType,
        CompartmentsOption _compartmentsOption,
        RetrieveStatisticsOption _retrieveStatistics,
        RetrieveParametersOption _retrieveParameters,
        RetrieveCovariatesOption _retrieveCovariates,
        ForceUgPerLiterOption _forceUgPerLiter)
    : m_parameterType(_parameterType), m_compartmentsOption(_compartmentsOption),
      m_retrieveStatistics(_retrieveStatistics), m_retrieveParameters(_retrieveParameters),
      m_retrieveCovariates(_retrieveCovariates), m_forceUgPerLiterOption(_forceUgPerLiter)
{
}

ComputingTrait::~ComputingTrait() = default;

void ComputingTraits::addTrait(std::unique_ptr<ComputingTrait> _trait)
{
    m_traits.push_back(std::move(_trait));
}


ComputingTrait::ComputingTrait(RequestResponseId _id) : m_id(std::move(_id)) {}


RequestResponseId ComputingTrait::getId() const
{
    return m_id;
}


ComputingTraitStandard::ComputingTraitStandard(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        double _nbPointsPerHour,
        ComputingOption _computingOption)
    : ComputingTrait(std::move(_id)), m_computingOption(_computingOption), m_start(_start), m_end(_end),
      m_nbPointsPerHour(_nbPointsPerHour)
{
}

ComputingTraitStandard::~ComputingTraitStandard() = default;


ComputingOption ComputingTraitStandard::getComputingOption() const
{
    return m_computingOption;
}


const Tucuxi::Common::DateTime& ComputingTraitStandard::getStart() const
{
    return m_start;
}


const Tucuxi::Common::DateTime& ComputingTraitStandard::getEnd() const
{
    return m_end;
}


double ComputingTraitStandard::getNbPointsPerHour() const
{
    return m_nbPointsPerHour;
}

ComputingStatus ComputingTraitStandard::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    // A ComputingTraitStandard is not a final class, so this method should not be called
    FINAL_UNUSED_PARAMETER(_computingComponent);
    FINAL_UNUSED_PARAMETER(_request);
    FINAL_UNUSED_PARAMETER(_response);
    return ComputingStatus::ComputingTraitStandardShouldNotBeCalled;
}

ComputingStatus ComputingTraitStandard::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    // A ComputingTraitStandard is not a final class, so this method should not be called
    FINAL_UNUSED_PARAMETER(_computingComponent);
    FINAL_UNUSED_PARAMETER(_request);
    FINAL_UNUSED_PARAMETER(_response);
    return ComputingStatus::ComputingTraitStandardShouldNotBeCalled;
}


ComputingTraitAdjustment::ComputingTraitAdjustment(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        double _nbPointsPerHour,
        ComputingOption _computingOption,
        Tucuxi::Common::DateTime _adjustmentTime,
        BestCandidatesOption _candidatesOption,
        LoadingOption _loadingOption,
        RestPeriodOption _restPeriodOption,
        SteadyStateTargetOption _steadyStateTargetOption,
        TargetExtractionOption _targetExtractionOption,
        FormulationAndRouteSelectionOption _formulationAndRouteSelectionOption)
    : ComputingTraitStandard(std::move(_id), _start, _end, _nbPointsPerHour, _computingOption),
      m_adjustmentTime(_adjustmentTime), m_bestCandidatesOption(_candidatesOption), m_loadingOption(_loadingOption),
      m_restPeriodOption(_restPeriodOption), m_steadyStateTargetOption(_steadyStateTargetOption),
      m_targetExtractionOption(_targetExtractionOption),
      m_formulationAndRouteSelectionOption(_formulationAndRouteSelectionOption)
{
}


Tucuxi::Common::DateTime ComputingTraitAdjustment::getAdjustmentTime() const
{
    return m_adjustmentTime;
}


BestCandidatesOption ComputingTraitAdjustment::getBestCandidatesOption() const
{
    return m_bestCandidatesOption;
}

FormulationAndRouteSelectionOption ComputingTraitAdjustment::getFormulationAndRouteSelectionOption() const
{
    return m_formulationAndRouteSelectionOption;
}


SteadyStateTargetOption ComputingTraitAdjustment::getSteadyStateTargetOption() const
{
    return m_steadyStateTargetOption;
}

LoadingOption ComputingTraitAdjustment::getLoadingOption() const
{
    return m_loadingOption;
}

RestPeriodOption ComputingTraitAdjustment::getRestPeriodOption() const
{
    return m_restPeriodOption;
}

TargetExtractionOption ComputingTraitAdjustment::getTargetExtractionOption() const
{
    return m_targetExtractionOption;
}


ComputingTraitConcentration::ComputingTraitConcentration(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        double _nbPointsPerHour,
        ComputingOption _computingOption)
    : ComputingTraitStandard(std::move(_id), _start, _end, _nbPointsPerHour, _computingOption)
{
}


ComputingTraitPercentiles::ComputingTraitPercentiles(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        PercentileRanks _ranks,
        double _nbPointsPerHour,
        ComputingOption _computingOption,
        ComputingAborter* _aborter)
    : ComputingTraitStandard(std::move(_id), _start, _end, _nbPointsPerHour, _computingOption),
      m_ranks(std::move(_ranks)), m_aborter(_aborter)
{
}


ComputingTraitAtMeasures::ComputingTraitAtMeasures(RequestResponseId _id, ComputingOption _computingOption)
    : ComputingTrait(std::move(_id)), m_computingOption(_computingOption)
{
}

ComputingOption ComputingTraitAtMeasures::getComputingOption() const
{
    return m_computingOption;
}


ComputingTraitSinglePoints::ComputingTraitSinglePoints(
        RequestResponseId _id, std::vector<Tucuxi::Common::DateTime> _times, ComputingOption _computingOption)
    : ComputingTrait(std::move(_id)), m_times(std::move(_times)), m_computingOption(_computingOption)
{
}

ComputingOption ComputingTraitSinglePoints::getComputingOption() const
{
    return m_computingOption;
}


const std::vector<Tucuxi::Common::DateTime>& ComputingTraitSinglePoints::getTimes() const
{
    return m_times;
}


ComputingStatus ComputingTraitConcentration::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitPercentiles::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitAdjustment::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitAtMeasures::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitSinglePoints::compute(
        ComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitConcentration::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitPercentiles::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitAdjustment::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitAtMeasures::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingStatus ComputingTraitSinglePoints::compute(
        MultiComputingComponent& _computingComponent,
        const ComputingRequest& _request,
        std::unique_ptr<ComputingResponse>& _response) const
{
    return _computingComponent.compute(this, _request, _response);
}



} // namespace Core
} // namespace Tucuxi
