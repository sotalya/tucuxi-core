/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "computingtrait.h"

#include "tucucore/computingcomponent.h"

namespace Tucuxi {
namespace Core {


ComputingOption::ComputingOption(PredictionParameterType _parameterType,
        CompartmentsOption _compartmentsOption, RetrieveStatisticsOption _retrieveStatistics, RetrieveParametersOption _retrieveParameters, RetrieveCovariatesOption _retrieveCovariates) :
    m_parameterType(_parameterType),
    m_compartmentsOption(_compartmentsOption),
    m_retrieveStatistics(_retrieveStatistics),
    m_retrieveParameters(_retrieveParameters),
    m_retrieveCovariates(_retrieveCovariates)
{

}

ComputingTrait::~ComputingTrait()
{

}

void ComputingTraits::addTrait(std::unique_ptr<ComputingTrait> _trait)
{
    m_traits.push_back(std::move(_trait));
}


ComputingTrait::ComputingTrait(RequestResponseId _id) : m_id(_id)
{
}


RequestResponseId ComputingTrait::getId() const
{
    return m_id;
}


ComputingTraitStandard::ComputingTraitStandard(RequestResponseId _id,
                        Tucuxi::Common::DateTime _start,
                        Tucuxi::Common::DateTime _end,
                        double _nbPointsPerHour,
                        ComputingOption _computingOption) :
    ComputingTrait(_id),
    m_computingOption(_computingOption), m_start(_start), m_end(_end), m_nbPointsPerHour(_nbPointsPerHour)
{
}

ComputingTraitStandard::~ComputingTraitStandard()
{

}


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

ComputingResult ComputingTraitStandard::compute(ComputingComponent &_computingComponent,
                                const ComputingRequest &_request,
                        std::unique_ptr<ComputingResponse> &_response) const
{
    // A ComputingTraitStandard is not a final class, so this method should not be called
    FINAL_UNUSED_PARAMETER(_computingComponent);
    FINAL_UNUSED_PARAMETER(_request);
    FINAL_UNUSED_PARAMETER(_response);
    return ComputingResult::ComputingTraitStandardShouldNotBeCalled;
}


ComputingTraitAdjustment::ComputingTraitAdjustment(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        double _nbPointsPerHour,
        ComputingOption _computingOption,
        Tucuxi::Common::DateTime _adjustmentTime,
        BestCandidatesOption _adjustmentOption,
        LoadingOption _loadingOption,
        RestPeriodOption _restPeriodOption,
        SteadyStateTargetOption _steadyStateTargetOption,
        TargetExtractionOption _targetExtractionOption,
        FormulationAndRouteSelectionOption _formulationAndRouteSelectionOption) :
    ComputingTraitStandard(_id, _start, _end, _nbPointsPerHour, _computingOption),
    m_adjustmentTime(_adjustmentTime),
    m_bestCandidatesOption(_adjustmentOption),
    m_loadingOption(_loadingOption),
    m_restPeriodOption(_restPeriodOption),
    m_steadyStateTargetOption(_steadyStateTargetOption),
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


ComputingTraitConcentration::ComputingTraitConcentration(RequestResponseId _id,
                             Tucuxi::Common::DateTime _start,
                             Tucuxi::Common::DateTime _end,
                             double _nbPointsPerHour,
                             ComputingOption _computingOption) :
    ComputingTraitStandard(_id, _start, _end, _nbPointsPerHour, _computingOption)
{
}


ComputingTraitPercentiles::ComputingTraitPercentiles(RequestResponseId _id,
                           Tucuxi::Common::DateTime _start,
                           Tucuxi::Common::DateTime _end,
                           const PercentileRanks &_ranks,
                           double _nbPointsPerHour,
                           ComputingOption _computingOption,
                           ComputingAborter *_aborter) :
    ComputingTraitStandard(_id, _start, _end, _nbPointsPerHour, _computingOption), m_ranks(_ranks), m_aborter(_aborter)
{
}


ComputingTraitAtMeasures::ComputingTraitAtMeasures(RequestResponseId _id, ComputingOption _computingOption)
    : ComputingTrait(_id), m_computingOption(_computingOption)
{
}

ComputingOption ComputingTraitAtMeasures::getComputingOption() const
{
    return m_computingOption;
}


ComputingTraitSinglePoints::ComputingTraitSinglePoints(RequestResponseId _id,
                            std::vector<Tucuxi::Common::DateTime> _times,
                            ComputingOption _computingOption) :
    ComputingTrait (_id), m_times(_times), m_computingOption(_computingOption)
{
}

ComputingOption ComputingTraitSinglePoints::getComputingOption() const
{
    return m_computingOption;
}


const std::vector<Tucuxi::Common::DateTime> &ComputingTraitSinglePoints::getTimes() const
{
    return m_times;
}


ComputingResult ComputingTraitConcentration::compute(
        ComputingComponent &_computingComponent,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingResult ComputingTraitPercentiles::compute(
        ComputingComponent &_computingComponent,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingResult ComputingTraitAdjustment::compute(
        ComputingComponent &_computingComponent,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingResult ComputingTraitAtMeasures::compute(
        ComputingComponent &_computingComponent,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response) const
{
    return _computingComponent.compute(this, _request, _response);
}


ComputingResult ComputingTraitSinglePoints::compute(
        ComputingComponent &_computingComponent,
        const ComputingRequest &_request,
        std::unique_ptr<ComputingResponse> &_response) const
{
    return _computingComponent.compute(this, _request, _response);
}




}
}
