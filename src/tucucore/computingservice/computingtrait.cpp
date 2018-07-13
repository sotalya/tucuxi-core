/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "computingtrait.h"

#include "tucucore/corecomponent.h"

namespace Tucuxi {
namespace Core {


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
                        PredictionParameterType _type,
                        Tucuxi::Common::DateTime _start,
                        Tucuxi::Common::DateTime _end,
                        const CycleSize _cycleSize,
                        ComputingOption _processingOption) :
    ComputingTrait(_id),
    m_type(_type), m_processingOption(_processingOption), m_start(_start), m_end(_end), m_cycleSize(_cycleSize)
{
}


PredictionParameterType ComputingTraitStandard::getType() const
{
    return m_type;
}


ComputingOption ComputingTraitStandard::getComputingOption() const
{
    return m_processingOption;
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


ComputingTraitAdjustment::ComputingTraitAdjustment(RequestResponseId _id,
                          PredictionParameterType _type,
                          Tucuxi::Common::DateTime _start,
                          Tucuxi::Common::DateTime _end,
                          const CycleSize _cycleSize,
                          ComputingOption _processingOption,
                          Tucuxi::Common::DateTime _adjustmentTime,
                          AdjustmentOption _adjustmentOption) :
    ComputingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption),
    m_adjustmentTime(_adjustmentTime),
    m_adjustmentOption(_adjustmentOption)
{
}


const Tucuxi::Common::DateTime& ComputingTraitAdjustment::getAdjustmentTime() const
{
    return m_adjustmentTime;
}


AdjustmentOption ComputingTraitAdjustment::getAdjustmentOption() const
{
    return m_adjustmentOption;
}


ComputingTraitConcentration::ComputingTraitConcentration(RequestResponseId _id,
                             PredictionParameterType _type,
                             Tucuxi::Common::DateTime _start,
                             Tucuxi::Common::DateTime _end,
                             const CycleSize _cycleSize,
                             ComputingOption _processingOption) :
    ComputingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption)
{
}


ComputingResult ComputingTraitConcentration::compute(CoreComponent &_coreComponent) const
{     
    return ComputingResult::Error;
}


ComputingTraitPercentiles::ComputingTraitPercentiles(RequestResponseId _id,
                           PredictionParameterType _type,
                           Tucuxi::Common::DateTime _start,
                           Tucuxi::Common::DateTime _end,
                           const PercentileRanks &_ranks,
                           const CycleSize _cycleSize,
                           ComputingOption _processingOption) :
    ComputingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption), m_ranks(_ranks)
{
}


ComputingTraitAtMeasures::ComputingTraitAtMeasures(RequestResponseId _id, ComputingOption _processingOption) 
    : ComputingTrait(_id), m_processingOption(_processingOption)
{
}

}
}
