/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H
#define TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <typeinfo>
#include <string>
#include <map>

#include "tucucommon/interface.h"
#include "tucucommon/component.h"

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Signature of a component factory function.
/// \return A pointer on one of the component's interface
typedef Interface* (*TComponentFactory)();

/// \ingroup TucuCommon
/// \brief Singleton managing the creation and the registration of Tucuxi's components.
///
/// This class act as the entry point of our "Component Oriented Programming" framework. 
/// It offers services to:
///   1. Register component factories
///   2. Create instances from registered factories
///   3. Register exisint component to make them available to the rest of the software
///   4. Retrieve registered components.
///
/// \sa Interface, Component
class ComponentManager
{
public:
    /// \brief The only way to get a hold on the component manager
    /// \return The component manager itself
    static ComponentManager* getInstance();

    /// \brief Register a factory and associate it with a name.
    /// \param _name The name that will allow creating instances via method createComponent
    /// \param _pFactory A pointer to a function that "knows" how to create a component of the given type.
    /// \return None
    void registerComponentFactory(const std::string &_name, TComponentFactory _pFactory);

    /// \brief Register a component so to make it available globally to the rest of the application.
    /// \param _name The name that will allow other parts of the software to retrieve the component.
    /// \param _pComponent A pointer to the interface of an existing component.
    /// \return None
    void registerComponent(const std::string &_name, Interface *_pComponent);

    /// \brief Unregister a previously registered component.
    /// \param _name The name of the component
    /// \return None
    void unregisterComponent(const std::string &_name);

    /// \brief Create an instance of a component from a previously registered factory.
    /// \param _factoryName The name of the factory.
    /// \param T The type of the desired interface
    /// \return A pointer on the specified interface of the newly created component
    template <typename T> T* createComponent(const std::string &_factoryName)
    {
        if (m_factories.end() != m_factories.find(_factoryName))
        {
            Interface *cmp = m_factories[_factoryName]();
            if (cmp != nullptr) {
                return cmp->getInterface<T>();
            }
        }
        return nullptr;
    }

    /// \brief Get a previously registered component.
    /// \param _name The name of the component.
    /// \param T The type of the desired interface
    /// \return A pointer on the specified interface of the specified component
    template <typename T> T* getComponent(const std::string &_name)
    {
        if (m_components.end() != m_components.find(_name))
        {
            Interface* itf = m_components[_name];
            if (itf != nullptr) {
                return itf->getInterface<T>();
            }
        }
        return nullptr;
    }

private:
    /// \brief Constructor. Used internal to create the singleton instance.
    ComponentManager();

    std::map<std::string, TComponentFactory> m_factories;   /// The list of registered factories
    std::map<std::string, Interface*> m_components;         /// The lisf of registered components
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_COMPONENTMANAGER_H
