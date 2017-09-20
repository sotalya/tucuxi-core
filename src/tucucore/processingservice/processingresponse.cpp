/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "processingresponse.h"


namespace Tucuxi {
namespace Core {


ProcessingResponse::ProcessingResponse(RequestResponseId _id) :
    m_id(_id)
{

}

RequestResponseId ProcessingResponse::getId() const
{
    return m_id;
}

void ProcessingResponse::addResponse(std::unique_ptr<SingleProcessingResponse> _response)
{
    m_responses.push_back(std::move(_response));
}

}
}
