/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <utility>

#include "computingresponse.h"


namespace Tucuxi {
namespace Core {


ComputingResponse::ComputingResponse(RequestResponseId _id) :
    m_id(std::move(_id))
{

}
RequestResponseId ComputingResponse::getId() const
{
    return m_id;
}

void ComputingResponse::addResponse(std::unique_ptr<ComputedData> _response)
{
    m_data = std::move(_response);
}

void ComputingResponse::setComputingTimeInSeconds(std::chrono::duration<double> _computingTime)
{
    m_computingTimeInSeconds = _computingTime;
}

std::chrono::duration<double> ComputingResponse::getComputingTimeInSeconds() const
{
    return m_computingTimeInSeconds;
}


void ComputingResponse::setComputingStatus(ComputingStatus _result)
{
    m_computingResult = _result;
}

ComputingStatus ComputingResponse::getComputingStatus() const
{
    return m_computingResult;
}


ComputedData::ComputedData(RequestResponseId _id) :
    m_id(std::move(_id))
{

}

ComputedData::~ComputedData() = default;

RequestResponseId ComputedData::getId() const
{
    return m_id;
}

}
}
