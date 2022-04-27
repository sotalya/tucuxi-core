//@@lisence@@

#ifndef TUCUXI_TUCUCOMMON_INTERFACE_H
#define TUCUXI_TUCUCOMMON_INTERFACE_H

#include <string>
#include <typeinfo>

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Abstract class for all interfaces.
/// \sa Component, ComponentManager
class Interface
{
public:
    /// \brief The method to use to switch between interface of a given component.
    /// \param T The type of the desired interface
    /// \return A pointer to the specified interface
    template<class T>
    T* getInterface()
    {
        std::string name = typeid(T*).name();
        Interface* itf = getInterface(name);
        if (itf != nullptr) {
            // Note that the actual "dynamic_cast" is done by components when calling Component::registerInterface !!!
            return static_cast<T*>(itf);
        }
        return nullptr;
    }

protected:
    /// \brief Abstract method allowing the templated version of getInterface to access other interfaces of the same component.
    /// \param _name The name of the interface as returned by typeid.
    /// \return A pointer to the specified interface
    virtual Interface* getInterface(const std::string& _name) = 0;
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_INTERFACE_H
