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


enum class ParameterVariabilityType
{
    Additive,
    Proportional,
    Exponential,
    None
};

class ParameterVariability
{
public:
//    ParameterVariability() : m_variabilityType(ParameterVariabilityType::None), m_value(0) {}
//    ParameterVariability(ParameterVariabilityType _type) : m_variabilityType(_type), m_value(0) {}
    ParameterVariability(ParameterVariabilityType _type = ParameterVariabilityType::None, Value _value = 0.0) : m_variabilityType(_type), m_value(_value) {}

    ParameterVariabilityType m_variabilityType;
    Value m_value;

};

/// \ingroup TucuCore
/// \brief Represents a pharmacokinetics parameter.
class ParameterDefinition : public PopulationValue
{
public:

public:    
    /// \brief Constructor
    /// \param _id The name of the parameter
    /// \param _name It's default value
    ParameterDefinition(const std::string _id, Value _value, ParameterVariability _variabilityType)
        : PopulationValue(_id, _value, nullptr),
          m_variability(_variabilityType)
    {}

    ParameterDefinition(const std::string _id, Value _value)
        : PopulationValue(_id, _value, nullptr)
    {}


    ParameterDefinition(const std::string _id, Value _value, ParameterVariabilityType _variabilityType)
        : PopulationValue(_id, _value, nullptr),
          m_variability(ParameterVariability(_variabilityType))
    {}

    ParameterDefinition(const std::string _name, Value _value, Operation* _operation, ParameterVariability _variabilityType)
        : PopulationValue(_name, _value, _operation),
          m_variability(_variabilityType)
    {}

    /// \brief Get the parameter value
    /// \return Returns the parameter value
    //Value getValue() const { return m_value; }

    const std::string& getName() const { return m_id; }
    bool isVariable() const { return m_variability.m_variabilityType != ParameterVariabilityType::None; }
    ParameterVariability getVariability() const { return m_variability; }

private:

    ParameterVariability m_variability;
    Unit m_unit;
};

/// \brief A list of parameters
typedef std::vector<std::unique_ptr<ParameterDefinition> > ParameterDefinitions;

class ParameterSetDefinition
{
public:

    void addParameter(std::unique_ptr<ParameterDefinition> & _parameter) { m_parameters.push_back(std::move(_parameter));}

    void addAnalyteId(std::string _analyteId) { m_analyteIds.push_back(_analyteId);}


protected:

    ParameterDefinitions m_parameters;
    std::vector<std::string> m_analyteIds;
};


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
            m_parameters.push_back(Parameter(**it));
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
