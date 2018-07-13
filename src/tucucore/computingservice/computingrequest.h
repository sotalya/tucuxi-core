/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef COMPUTINGREQUEST_H
#define COMPUTINGREQUEST_H

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
                      const ComputingTraits& _computingTraits);

    RequestResponseId getId() const;
    const DrugModel& getDrugModel() const;
    const DrugTreatment& getDrugTreatment() const;
    const ComputingTraits& getComputingTraits() const;

protected:
    RequestResponseId m_id;
    const DrugModel &m_drugModel;
    const DrugTreatment &m_drugTreatment;
    const ComputingTraits & m_computingTraits;
};

}
}

#endif // COMPUTINGREQUEST_H
