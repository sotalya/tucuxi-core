/*
* Copyright (C) 2017 Tucuxi SA
*/
#include "processingtrait.h"


namespace Tucuxi {
namespace Core {



void ProcessingTraits::addTrait(std::unique_ptr<ProcessingTrait> _trait)
{
    m_traits.push_back(std::move(_trait));
}

const std::vector<std::unique_ptr<ProcessingTrait> > &ProcessingTraits::getTraits() const
{
    return m_traits;
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
                        int _nbPoints,
                        ProcessingOption _processingOption) :
    ProcessingTrait(_id),
    m_type(_type), m_processingOption(_processingOption), m_start(_start), m_end(_end), m_nbPoints(_nbPoints)
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

int ProcessingTraitStandard::getNbPoints() const
{
    return m_nbPoints;
}

ProcessingTraitAdjustment::ProcessingTraitAdjustment(RequestResponseId _id,
                          PredictionParameterType _type,
                          Tucuxi::Common::DateTime _start,
                          Tucuxi::Common::DateTime _end,
                          int _nbPoints,
                          ProcessingOption _processingOption,
                          Tucuxi::Common::DateTime _adjustmentTime,
                          AdjustmentOption _adjustmentOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption),
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
                             int _nbPoints,
                             ProcessingOption _processingOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption)
{

}

ProcessingTraitPercentiles::ProcessingTraitPercentiles(RequestResponseId _id,
                           PredictionParameterType _type,
                           Tucuxi::Common::DateTime _start,
                           Tucuxi::Common::DateTime _end,
                           const PercentileRanks &_ranks,
                           int _nbPoints,
                           ProcessingOption _processingOption) :
    ProcessingTraitStandard(_id, _type, _start, _end, _nbPoints, _processingOption), m_ranks(_ranks)
{

}


ProcessingTraitAtMeasures::ProcessingTraitAtMeasures(RequestResponseId _id,
                          ProcessingOption _processingOption
                          ) : ProcessingTrait(_id), m_processingOption(_processingOption)
{

}
}
}
