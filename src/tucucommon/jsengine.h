/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_TUCUCOMMON_JSENGINE_H
#define TUCUXI_TUCUCOMMON_JSENGINE_H

#include <map>
#include <memory>
#include <string>
#include <thread>

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
    template<typename T>
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

    // A potential idea to save initialization time.
    // However we should clear the engine, just keeping the registered functions
    // static std::map<std::thread::id, std::unique_ptr<JSEngine> > sm_enginePerThread;
private:
    std::unique_ptr<CTinyJS> m_pEngine; /// The "wrapped" Tiny JS script engine
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_JSENGINE_H
