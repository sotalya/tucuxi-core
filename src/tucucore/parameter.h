/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PARAMETER_H
#define TUCUXI_CORE_PARAMETER_H

#include <string>
#include <vector>
#include <memory>

#include "tucucore/definitions.h"
#include "tucucore/timedevent.h"
#include "tucucore/operation.h"
#include "tucucommon/general.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Represents a pharmacokinetics parameter.
class ParameterDefinition : public PopulationValue
{
public:
    enum class ErrorModel { Additive, Proportional, Exponential, None };

public:    
    /// \brief Constructor
    /// \param _name The name of the parameter
    /// \param _name It's default value
    ParameterDefinition(const std::string _name, Value _value, ErrorModel _errType)
        : PopulationValue(_name, _value, nullptr),
          m_isVariable(false),
          m_errorModel(_errType)
    {}

    /// \brief Get the parameter value
    /// \return Returns the parameter value
    //Value getValue() const { return m_value; }

    const std::string& getName() const { return m_id; }
    bool isVariable() const { return m_isVariable; }
    ErrorModel getErrorModel() const { return m_errorModel; }

private:
//    std::string m_name;      /// Name like "CL" or "V1"
//    Value m_value;           /// The value (0.0 or 1.0 in case of booleans)
    bool m_isVariable;       /// Indicates whether there is an eta on this parameter
    ErrorModel m_errorModel;

//    Operation *m_operation; /// Potential operation from covariates

    //error_type_enum m_error_type;
};

/// \brief A list of parameters
typedef std::vector<ParameterDefinition> ParameterDefinitions;

class Parameter : public IndividualValue<ParameterDefinition>
{
public:
    Parameter(const ParameterDefinition &_def) :
        IndividualValue<ParameterDefinition>(_def)
         {
        m_value = _def.getValue();
    }
    void applyEta(Deviation eta);
    Value getValue() const { return m_value; }
    bool isVariable() { return m_definition.isVariable(); }    

private:
    Value m_value;
};
typedef std::vector<Parameter> Parameters;

class ParameterSetEvent : public TimedEvent
{
public:
    ParameterSetEvent(const DateTime& _date, const ParameterDefinitions &_definitions)
        : TimedEvent(_date)
    {
        ParameterDefinitions::const_iterator it;
        for (it = _definitions.begin(); it != _definitions.end(); it++) {
            m_parameters.push_back(Parameter(*it));
        }
    }

    void applyEtas(const Etas& _etas);
    Parameters::const_iterator begin() const { return m_parameters.begin(); }
    Parameters::const_iterator end() const { return m_parameters.end(); }
    void setValue(int _index, Value _value) { TMP_UNUSED_PARAMETER(_index); TMP_UNUSED_PARAMETER(_value);}
    Value getValue(int _index) const { return m_parameters[_index].getValue(); }
    int size() const { return static_cast<int>(m_parameters.size()); }    

private:
    Parameters m_parameters;
};
typedef std::unique_ptr<const ParameterSetEvent> ParameterSetEventPtr;

class ParameterSetSeries
{
public:
    ParameterSetEventPtr getAtTime(const DateTime &_date, const Etas &_etas = Etas(0)) const;
    void addParameterSetEvent(ParameterSetEvent parameterSetEvent);

private:
    std::vector<ParameterSetEvent> m_parameterSets;
};


}
}
#endif // TUCUXI_CORE_PARAMETER_H
