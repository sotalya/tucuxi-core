/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PROCESSINGREQUEST_H
#define PROCESSINGREQUEST_H

#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/computingservice/computingtrait.h"

namespace Tucuxi {
namespace Core {


class ComputingRequest
{
public:
    ComputingRequest(RequestResponseId _id,
                      const DrugModel& _drugModel,
                      const DrugTreatment& _drugTreatment,
                      const ComputingTraits& _processingTraits);

    RequestResponseId getId() const;
    const DrugModel& getDrugModel() const;
    const DrugTreatment& getDrugTreatment() const;
    const ComputingTraits& getComputingTraits() const;

protected:
    RequestResponseId m_id;
    const DrugModel &m_drugModel;
    const DrugTreatment &m_drugTreatment;
    const ComputingTraits & m_processingTraits;
};

}
}

#endif // PROCESSINGREQUEST_H
