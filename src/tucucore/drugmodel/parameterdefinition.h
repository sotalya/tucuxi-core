/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PARAMETERDEFINITION_H
#define PARAMETERDEFINITION_H

#include <string>
#include <memory>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"
#include "tucucore/timedevent.h"
#include "tucucommon/general.h"

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
    ParameterVariability(ParameterVariabilityType _type = ParameterVariabilityType::None, Value _value = 0.0) : m_type(_type), m_value(_value) {}

    ParameterVariabilityType getType() const { return m_type; }
    Value getValue() const { return m_value; }

private:
    ParameterVariabilityType m_type;
    Value m_value;
};

/// \ingroup TucuCore
/// \brief Represents a pharmacokinetics parameter.
class ParameterDefinition : public PopulationValue
{
public:
    /// \brief Constructor
    /// \param _id The name of the parameter
    /// \param _name Its default value
    ParameterDefinition(const std::string _id, Value _value, ParameterVariability _variabilityType);

    ParameterDefinition(const std::string _id, Value _value);

    ParameterDefinition(const std::string _id, Value _value, ParameterVariabilityType _variabilityType);

    ParameterDefinition(const std::string _name, Value _value, Operation* _operation, ParameterVariability _variabilityType);

    virtual ~ParameterDefinition();

    /// \brief Get the parameter value
    /// \return Returns the parameter value
    //Value getValue() const { return m_value; }

    bool isVariable() const { return m_variability.getType() != ParameterVariabilityType::None; }
    ParameterVariability getVariability() const { return m_variability; }

private:
    ParameterVariability m_variability;
    Unit m_unit;
};

/// \brief A list of parameters
typedef std::vector<std::unique_ptr<ParameterDefinition> > ParameterDefinitions;



/// \brief Parameter the context of an Operable Graph Manager. The timing information is missing since it is handled by
///        the parameter set itself.
class ParameterEvent : public IndividualValue<ParameterDefinition>, public Operable
{
public:
    /// \brief Remove the default constructor.
    ParameterEvent() = delete;
    /// \brief Create a change in a parameter given a reference to the desired parameter and the new value.
    /// \param _parameterDef Parameter definition that hass changed.
    /// \param _value New value of the parameter.
    ParameterEvent(const ParameterDefinition &_parameterDef, Value _value)
        : IndividualValue(_parameterDef), Operable(_value)
    {}

    /// \brief Get the associated operation.
    /// \return Reference to the associated operation.
    virtual Operation &getOperation() const override { return m_definition.getOperation(); }

    /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
    ///        OperableGraphManager.
    /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
    /// \return True if the evaluation could be performed, false in case of errors.
    virtual bool evaluate(const OperableGraphManager &_graphMgr) override;

    /// \brief Return the identifier of the parameter involved in the change.
    /// \return Identifier of parameter involved in the change.
    std::string getId() const { return m_definition.getId(); }
};


class Correlation
{
public:
    Correlation(std::string _parameter1, std::string _parameter2, Value _correlation) :
        m_correlation(_correlation)
    {
        m_parameterId.push_back(_parameter1);
        m_parameterId.push_back(_parameter2);
    }

protected:
    Value m_correlation;
    std::vector<std::string> m_parameterId;
};

typedef std::vector<Correlation> Correlations;

class InterParameterSetCorrelation
{
public:
    InterParameterSetCorrelation(
            std::string _analyteSetId1,
            std::string _analyteSetId2,
            std::string _parameterId1,
            std::string _parameterId2,
            Value _correlation) :
        m_correlation(_correlation)
    {
        m_analyteSetId.push_back(_analyteSetId1);
        m_analyteSetId.push_back(_analyteSetId2);
        m_parameterId.push_back(_parameterId1);
        m_parameterId.push_back(_parameterId2);
    }

protected:
    Value m_correlation;
    std::vector<std::string> m_analyteSetId;
    std::vector<std::string> m_parameterId;
};

typedef std::vector<InterParameterSetCorrelation> InterParameterSetCorrelations;

class ParameterSetDefinition
{
public:
    void addParameter(std::unique_ptr<ParameterDefinition> & _parameter) { m_parameters.push_back(std::move(_parameter));}

    void addAnalyteId(std::string _analyteId) { m_analyteIds.push_back(_analyteId);}

    void addCorrelation(Correlation _correlation) { m_correlations.push_back(_correlation);}

    size_t getNbParameters() const { return m_parameters.size(); }
    const ParameterDefinition* getParameter(size_t _index) const {
        if (_index < m_parameters.size()) {
            return m_parameters.at(_index).get();
        }
        return nullptr;
    }

protected:
    ParameterDefinitions m_parameters;
    Correlations m_correlations;
    std::vector<std::string> m_analyteIds;
};


} // namespace Core
} // namespace Tucuxi


#endif // PARAMETERDEFINITION_H
