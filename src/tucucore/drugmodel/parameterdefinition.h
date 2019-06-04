/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef PARAMETERDEFINITION_H
#define PARAMETERDEFINITION_H

#include <string>
#include <memory>
#include <vector>

#include "tucucommon/utils.h"
#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"
#include "tucucore/timedevent.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class DrugModelChecker;

///
/// \brief The potential parameter variability types supported
///
enum class ParameterVariabilityType
{
    /// The parameter is fixed
    None = 0,

    /// The parameter follows a Normal distribution
    /// \warning Not supported
    Normal,

    /// The parameter follows a LogNormal distribution
    /// \warning Not supported
    LogNormal,

    /// The parameter has an additive variability
    Additive,

    /// The variability is proportional
    /// \warning Actually we use Exponential variability instead.
    Proportional,

    /// The variability is exponential
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

    bool isVariable() const {
        if (m_variability == nullptr) {
            return false;
        }
        return m_variability->getType() != ParameterVariabilityType::None;
    }

    const ParameterVariability& getVariability() const { return *m_variability; }

    /// \brief Set the validation operation
    /// \param _operation Operation used to validate the covariate value
    void setValidation(std::unique_ptr<Operation> _validation) { m_validation = std::move(_validation);}

    void setUnit(Unit _unit) { m_unit = _unit;}
    Unit getUnit() const { return m_unit;}

    INVARIANTS(
            INVARIANT(Invariants::INV_PARAMETERDEFINITION_0001, (m_id.size() > 0), "A parameter has no Id")
            INVARIANT(Invariants::INV_PARAMETERDEFINITION_0002, (m_variability != nullptr), Tucuxi::Common::Utils::strFormat("A parameter %s has no variability defined", m_id))
            )

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

    INVARIANTS(
            INVARIANT(Invariants::INV_CORRELATION_0001, (m_parameterId.size() == 2), "A correlation does not have 2 parameters")
            )

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

    INVARIANTS(
            INVARIANT(Invariants::INV_INTERPARAMETERSETCORRELATION_0001, (m_analyteSetId.size() == 2), "An inter-parameter set correlation does not have 2 analyte sets defined")
            INVARIANT(Invariants::INV_INTERPARAMETERSETCORRELATION_0002, (m_parameterId.size() == 2), "An inter-parameter set correlation does not have 2 parameters defined")
            )

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

    void addCorrelation(Correlation _correlation) { m_correlations.push_back(_correlation);}

    size_t getNbParameters() const { return m_parameters.size(); }
    const ParameterDefinition* getParameter(size_t _index) const {
        if (_index < m_parameters.size()) {
            return m_parameters.at(_index).get();
        }
        return nullptr;
    }

    const Correlations & getCorrelations() const { return m_correlations;}


    INVARIANTS(
            LAMBDA_INVARIANT(Invariants::INV_PARAMETERSETDEFINITION_0001, {bool ok = true;for(size_t i = 0; i < m_parameters.size(); i++) {ok &= m_parameters.at(i)->checkInvariants();} return ok;}, "There is an error in a parameter of an inter-parameter set correlation")
            LAMBDA_INVARIANT(Invariants::INV_PARAMETERSETDEFINITION_0002, {bool ok = true;for(size_t i = 0; i < m_correlations.size(); i++) {ok &= m_correlations.at(i).checkInvariants();} return ok;}, "There is an error in a correlation of an inter-parameter set correlation")
            )

protected:
    ParameterDefinitions m_parameters;
    Correlations m_correlations;

    friend DrugModelChecker;
};


} // namespace Core
} // namespace Tucuxi


#endif // PARAMETERDEFINITION_H
