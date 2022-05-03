//@@license@@

#include <utility>

#include "computingrequest.h"


namespace Tucuxi {
namespace Core {

ComputingRequest::ComputingRequest(
        RequestResponseId _id,
        const DrugModel& _drugModel,
        const DrugTreatment& _drugTreatment,
        std::unique_ptr<ComputingTrait> _computingTrait)
    : m_id(std::move(_id)), m_drugModel(_drugModel), m_drugTreatment(_drugTreatment)
{
    m_computingTraits = std::make_unique<ComputingTraits>();
    m_computingTraits->addTrait(std::move(_computingTrait));
}

RequestResponseId ComputingRequest::getId() const
{
    return m_id;
}

const DrugModel& ComputingRequest::getDrugModel() const
{
    return m_drugModel;
}

const DrugTreatment& ComputingRequest::getDrugTreatment() const
{
    return m_drugTreatment;
}

const ComputingTraits& ComputingRequest::getComputingTraits() const
{
    return *m_computingTraits;
}

} // namespace Core
} // namespace Tucuxi
