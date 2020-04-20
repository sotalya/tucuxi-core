/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "computingresponse.h"


namespace Tucuxi {
namespace Core {


ComputingResponse::ComputingResponse(RequestResponseId _id) :
    m_id(_id)
{

}

RequestResponseId ComputingResponse::getId() const
{
    return m_id;
}

void ComputingResponse::addResponse(std::unique_ptr<SingleComputingResponse> _response)
{
    m_responses.push_back(std::move(_response));
}

void ComputingResponse::setComputingTimeInSeconds(std::chrono::duration<double> _computingTime)
{
    m_computingTimeInSeconds = _computingTime;
}

std::chrono::duration<double> ComputingResponse::getComputingTimeInSeconds() const
{
    return m_computingTimeInSeconds;
}

SingleComputingResponse::SingleComputingResponse(RequestResponseId _id) :
    m_id(_id)
{

}

SingleComputingResponse::~SingleComputingResponse()
{
}

RequestResponseId SingleComputingResponse::getId() const
{
    return m_id;
}

}
}
