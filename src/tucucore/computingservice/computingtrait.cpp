/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "computingtrait.h"

#include "tucucore/computingcomponent.h"

namespace Tucuxi {
namespace Core {


ComputingOption::ComputingOption(
        PredictionParameterType _parameterType,
        CompartmentsOption _compartmentsOption) :
    m_parameterType(_parameterType),
    m_compartmentsOption(_compartmentsOption)
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
                        const CycleSize _cycleSize,
                        ComputingOption _computingOption) :
    ComputingTrait(_id),
    m_computingOption(_computingOption), m_start(_start), m_end(_end), m_cycleSize(_cycleSize)
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


CycleSize ComputingTraitStandard::getCycleSize() const
{
    return m_cycleSize;
}


ComputingTraitAdjustment::ComputingTraitAdjustment(
        RequestResponseId _id,
        Tucuxi::Common::DateTime _start,
        Tucuxi::Common::DateTime _end,
        const CycleSize _cycleSize,
        ComputingOption _computingOption,
        Tucuxi::Common::DateTime _adjustmentTime,
        AdjustmentOption _adjustmentOption,
        ChargingOption _chargingOption,
        RestPeriodOption _restPeriodOption,
        SteadyStateTargetOption _steadyStateTargetOption,
        TargetExtractionOption _targetExtractionOption) :
    ComputingTraitStandard(_id, _start, _end, _cycleSize, _computingOption),
    m_adjustmentTime(_adjustmentTime),
    m_adjustmentOption(_adjustmentOption),
    m_chargingOption(_chargingOption),
    m_restPeriodOption(_restPeriodOption),
    m_steadyStateTargetOption(_steadyStateTargetOption),
    m_targetExtractionOption(_targetExtractionOption)
{
}


Tucuxi::Common::DateTime ComputingTraitAdjustment::getAdjustmentTime() const
{
    return m_adjustmentTime;
}


AdjustmentOption ComputingTraitAdjustment::getAdjustmentOption() const
{
    return m_adjustmentOption;
}


ComputingTraitConcentration::ComputingTraitConcentration(RequestResponseId _id,
                             Tucuxi::Common::DateTime _start,
                             Tucuxi::Common::DateTime _end,
                             const CycleSize _cycleSize,
                             ComputingOption _computingOption) :
    ComputingTraitStandard(_id, _start, _end, _cycleSize, _computingOption)
{
}


ComputingTraitPercentiles::ComputingTraitPercentiles(RequestResponseId _id,
                           Tucuxi::Common::DateTime _start,
                           Tucuxi::Common::DateTime _end,
                           const PercentileRanks &_ranks,
                           const CycleSize _cycleSize,
                           ComputingOption _computingOption) :
    ComputingTraitStandard(_id, _start, _end, _cycleSize, _computingOption), m_ranks(_ranks)
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
