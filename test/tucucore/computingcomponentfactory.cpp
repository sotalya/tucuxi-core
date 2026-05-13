//@@license@@

#include "computingcomponentfactory.h"

#include "tucucore/computingcomponent.h"
#include "tucucore/metacomputingcomponent.h"
#include "tucucore/multicomputingcomponent.h"

#ifdef TUCU_COMPILE_MULTI
#include "computingcomponentcomparator.h"
#endif // TUCU_COMPILE_MULTI

ComputingComponentFactory::CreationMode ComputingComponentFactory::sm_currentMode =
        ComputingComponentFactory::CreationMode::Meta;

std::unique_ptr<Tucuxi::Core::IComputingService> ComputingComponentFactory::createComputingService()
{
    switch (sm_currentMode) {
    case CreationMode::Meta:
        return std::unique_ptr<Tucuxi::Core::IComputingService>(dynamic_cast<Tucuxi::Core::IComputingService*>(
                Tucuxi::Core::MetaComputingComponent::createComponent()));
    case CreationMode::Single:
        return std::unique_ptr<Tucuxi::Core::IComputingService>(
                dynamic_cast<Tucuxi::Core::IComputingService*>(Tucuxi::Core::ComputingComponent::createComponent()));
    case CreationMode::Multi:
        return std::unique_ptr<Tucuxi::Core::IComputingService>(dynamic_cast<Tucuxi::Core::IComputingService*>(
                Tucuxi::Core::MultiComputingComponent::createComponent()));
#ifdef TUCU_COMPILE_MULTI
    case CreationMode::Comparator:
        return std::unique_ptr<Tucuxi::Core::IComputingService>(dynamic_cast<Tucuxi::Core::IComputingService*>(
                Tucuxi::Core::ComputingComponentComparator::createComponent()));
#endif // TUCU_COMPILE_MULTI
    }
    return nullptr;
}

void ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode _mode)
{
    sm_currentMode = _mode;
}
