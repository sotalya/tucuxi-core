/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PROCESSINGREQUEST_H
#define PROCESSINGREQUEST_H

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/processingservice/processingtrait.h"

namespace Tucuxi {
namespace Core {


class ProcessingRequest
{
public:
    ProcessingRequest(RequestResponseId _id,
                      const DrugModel& _drugModel,
                      const DrugTreatment& _drugTreatment,
                      const ProcessingTraits& _processingTraits);

    RequestResponseId getId() const;
    const DrugModel& getDrugModel() const;
    const DrugTreatment& getDrugTreatment() const;
    const ProcessingTraits& getProcessingTraits() const;

protected:
    RequestResponseId m_id;
    const DrugModel &m_drugModel;
    const DrugTreatment &m_drugTreatment;
    const ProcessingTraits & m_processingTraits;
};

}
}

#endif // PROCESSINGREQUEST_H
