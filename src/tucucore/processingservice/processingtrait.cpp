/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "processingtrait.h"

#include "tucucore/corecomponent.h"

namespace Tucuxi {
namespace Core {


void ProcessingTraits::addTrait(std::unique_ptr<ProcessingTrait> _trait)
{
    m_traits.push_back(std::move(_trait));
}


ProcessingTrait::ProcessingTrait(RequestResponseId _id) : m_id(_id)
{
}


RequestResponseId ProcessingTrait::getId() const
{
    return m_id;
}


ProcessingTraitStandard::ProcessingTraitStandard(RequestResponseId _id,
                        PredictionParameterType _type,
                        Tucuxi::Common::DateTime _start,
                        Tucuxi::Common::DateTime _end,
                        const CycleSize _cycleSize,
                        ProcessingOption _processingOption) :
    ProcessingTrait(_id),
    m_type(_type), m_processingOption(_processingOption), m_start(_start), m_end(_end), m_cycleSize(_cycleSize)
{
}


PredictionParameterType ProcessingTraitStandard::getType() const
{
    return m_type;
}


ProcessingOption ProcessingTraitStandard::getProcessingOption() const
{
    return m_processingOption;
}


const Tucuxi::Common::DateTime& ProcessingTraitStandard::getStart() const
{
    return m_start;
}


const Tucuxi::Common::DateTime& ProcessingTraitStandard::getEnd() const
{
    return m_end;
}


CycleSize ProcessingTraitStandard::getCycleSize() const
{
    return m_cycleSize;
}


ProcessingTraitAdjustment::ProcessingTraitAdjustment(RequestResponseId _id,
                          PredictionParameterType _type,
                          Tucuxi::Common::DateTime _start,
                          Tucuxi::Common::DateTime _end,
                          const CycleSize _cycleSize,
                          ProcessingOption _processingOption,
                          Tucuxi::Common::DateTime _adjustmentTime,
                          AdjustmentOption _adjustmentOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption),
    m_adjustmentTime(_adjustmentTime),
    m_adjustmentOption(_adjustmentOption)
{
}


const Tucuxi::Common::DateTime& ProcessingTraitAdjustment::getAdjustmentTime() const
{
    return m_adjustmentTime;
}


AdjustmentOption ProcessingTraitAdjustment::getAdjustmentOption() const
{
    return m_adjustmentOption;
}


ProcessingTraitConcentration::ProcessingTraitConcentration(RequestResponseId _id,
                             PredictionParameterType _type,
                             Tucuxi::Common::DateTime _start,
                             Tucuxi::Common::DateTime _end,
                             const CycleSize _cycleSize,
                             ProcessingOption _processingOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption)
{
}


ProcessingResult ProcessingTraitConcentration::compute(CoreComponent &_coreComponent) const
{     
    return ProcessingResult::Error;
}


ProcessingTraitPercentiles::ProcessingTraitPercentiles(RequestResponseId _id,
                           PredictionParameterType _type,
                           Tucuxi::Common::DateTime _start,
                           Tucuxi::Common::DateTime _end,
                           const PercentileRanks &_ranks,
                           const CycleSize _cycleSize,
                           ProcessingOption _processingOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _cycleSize, _processingOption), m_ranks(_ranks)
{
}


ProcessingTraitAtMeasures::ProcessingTraitAtMeasures(RequestResponseId _id, ProcessingOption _processingOption) 
    : ProcessingTrait(_id), m_processingOption(_processingOption)
{
}

}
}
