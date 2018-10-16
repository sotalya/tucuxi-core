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
    None = 0,
    Normal,
    LogNormal,
    Additive,
    Proportional,
    Exponential
};

///
/// \brief The ParameterVariability class
/// The variability will usually only embed a single standard deviation.
/// However, it is possible to use more than one standard deviation if required.
///
class ParameterVariability
{
public:
    ParameterVariability(ParameterVariabilityType _type = ParameterVariabilityType::None, Value _value = 0.0) : m_type(_type) {
        m_values.push_back(_value);
    }

    ParameterVariability(ParameterVariabilityType _type, std::vector<Value> _values) : m_type(_type), m_values(_values) {
    }

    ParameterVariabilityType getType() const { return m_type; }
    Value getValue() const { return m_values.at(0); }
    std::vector<Value> getValues() const { return m_values;}

private:
    ParameterVariabilityType m_type;
    std::vector<Value> m_values;
};

/// \ingroup TucuCore
/// \brief Represents a pharmacokinetics parameter.
class ParameterDefinition : public PopulationValue
{
public:
    /// \brief Constructor
    /// \param _id The name of the parameter
    /// \param _name Its default value
    ParameterDefinition(std::string _id, Value _value, std::unique_ptr<ParameterVariability> _variabilityType);

    ParameterDefinition(std::string _id, Value _value);

    ParameterDefinition(std::string _id, Value _value, ParameterVariabilityType _variabilityType);

    ParameterDefinition(std::string _name, Value _value, Operation* _operation, std::unique_ptr<ParameterVariability> _variabilityType);

    ParameterDefinition(std::string _name, Value _value, Operation* _operation, ParameterVariabilityType _variabilityType);

    ~ParameterDefinition() override;

    /// \brief Get the parameter value
    /// \return Returns the parameter value
    //Value getValue() const { return m_value; }

    bool isVariable() const { return m_variability->getType() != ParameterVariabilityType::None; }
    const ParameterVariability& getVariability() const { return *m_variability; }

    /// \brief Set the validation operation
    /// \param _operation Operation used to validate the covariate value
    void setValidation(std::unique_ptr<Operation> _validation) { m_validation = std::move(_validation);}

private:
    std::unique_ptr<ParameterVariability> m_variability;
    Unit m_unit;

    /// \brief Operation to validate the value of the covariate
    std::unique_ptr<Operation> m_validation;
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
    Operation &getOperation() const override { return m_definition.getOperation(); }

    /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
    ///        OperableGraphManager.
    /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
    /// \return True if the evaluation could be performed, false in case of errors.
    bool evaluate(const OperableGraphManager &_graphMgr) override;

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

    Value getValue() const { return m_correlation;}
    std::string getParamId1() const { return m_parameterId[0];}
    std::string getParamId2() const { return m_parameterId[1];}

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
    void addParameter(std::unique_ptr<ParameterDefinition> _parameter) { m_parameters.push_back(std::move(_parameter));}

    void addAnalyteId(std::string _analyteId) { m_analyteIds.push_back(_analyteId);}

    void addCorrelation(Correlation _correlation) { m_correlations.push_back(_correlation);}

    size_t getNbParameters() const { return m_parameters.size(); }
    const ParameterDefinition* getParameter(size_t _index) const {
        if (_index < m_parameters.size()) {
            return m_parameters.at(_index).get();
        }
        return nullptr;
    }

    const Correlations & getCorrelations() const { return m_correlations;}

protected:
    ParameterDefinitions m_parameters;
    Correlations m_correlations;
    std::vector<std::string> m_analyteIds;
};


} // namespace Core
} // namespace Tucuxi


#endif // PARAMETERDEFINITION_H
