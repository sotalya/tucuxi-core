/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_PARAMETER_H
#define TUCUXI_MATH_PARAMETER_H

#include <string>
#include <vector>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Represents a pharmacokinetics parameter.
class Parameter
{
public:
    /// \brief Constructor
    /// @param _name The name of the parameter
    /// @param _name It's value
    Parameter(const std::string _name, Value _value)
        : m_name(_name), 
          m_value(_value), 
          m_isVariable(false) 
    {}

    /// \brief Get the parameter value
    /// @return Returns the parameter value
    Value getValue() const { return m_value; }

private:
    std::string m_name;     /// Name like "CL" or "V1"
    Value m_value;          /// The value (0.0 or 1.0 in case of booleans)	
    bool m_isVariable;      /// Indicates whether there is an eta on this parameter

    //error_type_enum m_error_type;
};

/// \brief A list of parameters
typedef std::vector<Parameter> ParameterList;

}
}
#endif // TUCUXI_MATH_PARAMETER_H