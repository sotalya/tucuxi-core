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
#include "tucucore/drugmodel/parameterdefinition.h"

struct TestParameterExtractor;

namespace Tucuxi {
namespace Core {

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

    // Make the test class friend, as this will allow us to manually check the available events.
    friend TestParameterExtractor;

private:
    Value m_value;
};

typedef std::vector<Parameter> Parameters;

class ParameterSetEvent : public TimedEvent
{
public:
    /// \brief Create an empty parameter set event at a given date.
    /// \param _date Timing information for the parameter set event.
    ParameterSetEvent(const DateTime &_date)
        : TimedEvent(_date)
    { }


    /// \brief Add a parameter event to the event set.
    /// \param _definition Parameter definition for the parameter event occurred.
    void addParameterEvent(const ParameterDefinition &_definition)
    {
        m_parameters.push_back(Parameter(_definition));
    }


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

    // Make the test class friend, as this will allow us to manually check the available events.
    friend TestParameterExtractor;

private:
    Parameters m_parameters;
};
typedef std::unique_ptr<const ParameterSetEvent> ParameterSetEventPtr;

class ParameterSetSeries
{
public:
    ParameterSetEventPtr getAtTime(const DateTime &_date, const Etas &_etas = Etas(0)) const;
    void addParameterSetEvent(const ParameterSetEvent &_parameterSetEvent);

    // Make the test class friend, as this will allow us to manually check the available events.
    friend TestParameterExtractor;

private:
    std::vector<ParameterSetEvent> m_parameterSets;
};


}
}
#endif // TUCUXI_CORE_PARAMETER_H
