/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "processingrequest.h"


namespace Tucuxi {
namespace Core {


ProcessingRequest::ProcessingRequest(RequestResponseId _id,
    const DrugModel& _drugModel,
    const DrugTreatment& _drugTreatment,
    const ProcessingTraits& _processingTraits) :
    m_id(_id),
    m_drugModel(_drugModel),
    m_drugTreatment(_drugTreatment),
    m_processingTraits(_processingTraits)
{
}

RequestResponseId ProcessingRequest::getId() const
{
    return m_id;
}

const DrugModel& ProcessingRequest::getDrugModel() const
{
    return m_drugModel;
}

const DrugTreatment& ProcessingRequest::getDrugTreatment() const
{
    return m_drugTreatment;
}

const ProcessingTraits& ProcessingRequest::getProcessingTraits() const
{
    return m_processingTraits;
}

}
}
