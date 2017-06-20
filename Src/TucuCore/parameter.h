#ifndef TUCUXI_MATH_PARAMETER_H
#define TUCUXI_MATH_PARAMETER_H

#include <string>
#include <vector>

#include "Definitions.h"

namespace Tucuxi {
namespace Math {

// Represents a pharmacokinetics parameter.
class Parameter
{
public:
	Parameter(const std::string _name, Float _value) 
		: m_name(_name), 
		  m_value(_value), 
		  m_isVariable(false) 
	{}

	Float getValue() const { return m_value; }

private:
	// Name like "CL" or "V1"
	std::string m_name;

	// If its a bool its treated at 0 or 1, so bool, integer and double 
	// are all converted to doubles. Nothing else is valid.
	Float m_value;

	// Indicates whether there is an eta on this parameter
	bool m_isVariable;

	//error_type_enum m_error_type;
};

typedef std::vector<Parameter> ParameterList;
}
}
#endif // TUCUXI_MATH_PARAMETER_H