/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCOMMON_JSENGINE_H
#define TUCUXI_TUCUCOMMON_JSENGINE_H

#include <string>
#include <memory>

class CTinyJS;

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief A simple javascript engine.
class JSEngine
{
public:
    /// \brief Default constructor
    JSEngine();

    /// \brief Destructor deleting pointers
    ~JSEngine();
    
    /// \brief Retrieve a variable managed by the engine.
    /// The variable may have been added via setVariable or during the execution of a script.
    /// \param _name The name of the variable
    /// \param _value Its value
    /// \return True if the variable was found
    template < typename T>
    bool getVariable(const std::string& _name, T& _value);

    /// \brief Create a new variable or change the value of an existing one.
    /// \param _name The name of the variable
    /// \param _value An expression to change the value of the variable
    /// \return True if the operation was successful
    bool setVariable(const std::string& _name, const std::string& _expression);
    
    /// \brief Execute the specified script
    /// \param _code The script to execute
    /// \return True if the execution of the given script was successfull
    bool evaluate(const std::string& _code);

    /// \brief Clear the internal dictionary of variables
    void reset();

private:
    std::unique_ptr<CTinyJS> m_pEngine; /// The "wrapped" Tiny JS script engine
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_JSENGINE_H
