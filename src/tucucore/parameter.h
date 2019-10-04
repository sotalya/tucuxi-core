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
#include "tucucore/parameterids.h"
#include "tucucore/drugmodel/analyte.h"

struct TestParameterExtractor;

namespace Tucuxi {
namespace Core {

class ParametersExtractor;

class Parameter : public IndividualValue<ParameterDefinition>
{
public:
    Parameter(const ParameterDefinition &_def, const Value _value) :
        IndividualValue<ParameterDefinition>(_def),
        m_value(_value)
    {
    }

    Parameter(Parameter && _other) = default;
    Parameter(const Parameter & _other) = default;
/*
    const Parameter& operator=(const Parameter& other) {
        if (this != &other) { // protect against invalid self-assignment
            m_value = other.m_value;
            this-
            m_definition = other.m_definition;
        }
        return *this;
    }
*/
    void applyEta(Deviation _eta);
    Value getValue() const { return m_value; }
    bool isVariable() const { return m_definition.isVariable(); }

    std::string getParameterId() const {return m_definition.getId();}

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
    {
        for (int i = 0; i < ParameterId::size; i++) {
            m_IdToIndex[i] = -1;
        }
    }

    void setEventTime(const DateTime &_date)
    {
        m_time = _date;
    }


    /// \brief Add a parameter event to the event set.
    /// \param _definition Parameter definition for the parameter event occurred.
    /// This function ensures the parameters are ordered correctly:
    /// - first the variable parameters, then the fixed parameters
    /// - Within a categoy, alphabetical order is respected.
    ///
    /// For instance, with Ka fixed, F fixed, CL variable, V variable, the storing will be:
    /// 1. CL
    /// 2. V
    /// 3. F
    /// 4. Ka
    ///
    /// Finaly, if the parameter is already in the set, its value is updated. This feature
    /// is useful to build the ParameterSetEvent after extraction of changing parameters
    /// in the ParametersExtractor.
    void addParameterEvent(const ParameterDefinition &_definition, Value _value);

    ParameterSetEvent(const DateTime& _date, const ParameterDefinitions &_definitions)
        : TimedEvent(_date)
    {
        ParameterDefinitions::const_iterator it;
        for (it = _definitions.begin(); it != _definitions.end(); it++) {
            addParameterEvent(**it, (*it)->getValue());
        }
    }


    void applyEtas(const Etas& _etas);
    Parameters::const_iterator begin() const { return m_parameters.begin(); }
    Parameters::const_iterator end() const { return m_parameters.end(); }

    Value getValue(ParameterId::Enum _id) const
    {
        int index = m_IdToIndex[_id];
        if (index >= 0) {
            return m_parameters[index].getValue();
        }
        return 0;
    }
    int size() const { return static_cast<int>(m_parameters.size()); }

    bool equals(const ParameterSetEvent *_other) const {
        if (m_parameters.size() != _other->m_parameters.size()) {
            return false;
        }
        for(size_t i = 0; i < m_parameters.size(); i++) {
            if (m_parameters[i].getValue() != _other->m_parameters[i].getValue()) {
                return false;
            }
            if (m_parameters[i].getParameterId() != _other->m_parameters[i].getParameterId()) {
                return false;
            }
        }
        return true;
    }

    // Make the test class friend, as this will allow us to manually check the available events.
    friend TestParameterExtractor;

private:
    Parameters m_parameters;
    int m_IdToIndex[ParameterId::size];
};
typedef std::unique_ptr<const ParameterSetEvent> ParameterSetEventPtr;

class ParameterSetSeries
{
public:
    ParameterSetEventPtr getAtTime(const DateTime &_date, const Etas &_etas = Etas(0)) const;
    void addParameterSetEvent(const ParameterSetEvent &_parameterSetEvent);

    // Make the test class friend, as this will allow us to manually check the available events.
    friend TestParameterExtractor;

    // To allow the ParametersExtractor to manipulate m_parameterSets
    friend ParametersExtractor;

// private:
    std::vector<ParameterSetEvent> m_parameterSets;
};


typedef std::map<AnalyteGroupId, ParameterSetSeries> GroupsParameterSetSeries;


} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_PARAMETER_H
