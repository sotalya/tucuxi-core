//@@license@@

#include "metacomputingcomponent.h"

#include "computingcomponent.h"
#include "computingservice/computingrequest.h"
#include "computingservice/computingresponse.h"
#include "multicomputingcomponent.h"

namespace Tucuxi {
namespace Core {


MetaComputingComponent::MetaComputingComponent()
{
    registerInterface(dynamic_cast<IComputingService*>(this));
}


Tucuxi::Common::Interface* MetaComputingComponent::createComponent()
{
    MetaComputingComponent* cmp = new MetaComputingComponent();

    cmp->initialize();

    return dynamic_cast<IComputingService*>(cmp);
}

Tucuxi::Common::Interface* MetaComputingComponent::getInterface(const std::string& _name)
{
    return Tucuxi::Common::Component::getInterfaceImpl(_name);
}

ComputingStatus MetaComputingComponent::compute(
        const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response)
{
    if (_request.getDrugModel().isSingleAnalyte()) {
        return m_computingComponent->compute(_request, _response);
    }
    return m_multiComputingComponent->compute(_request, _response);
}

void MetaComputingComponent::setPkModelCollection(std::shared_ptr<PkModelCollection> _collection)
{
    m_computingComponent->setPkModelCollection(_collection);
    m_multiComputingComponent->setPkModelCollection(_collection);
}

std::string MetaComputingComponent::getErrorString() const
{
    auto singleString = m_computingComponent->getErrorString();
    auto multiString = m_multiComputingComponent->getErrorString();
    if (multiString.empty()) {
        return "Single : " + singleString;
    }
    if (singleString.empty()) {
        return "Multi : " + multiString;
    }
    return "Single : " + singleString + ". Multi : " + multiString;
}

bool MetaComputingComponent::initialize()
{

    m_computingComponent =
            std::unique_ptr<IComputingService>(dynamic_cast<IComputingService*>(ComputingComponent::createComponent()));
    m_multiComputingComponent = std::unique_ptr<IComputingService>(
            dynamic_cast<IComputingService*>(MultiComputingComponent::createComponent()));
    return true;
}

} // namespace Core
} // namespace Tucuxi
