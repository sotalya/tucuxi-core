#ifndef DRUGDEFINITIONS_H
#define DRUGDEFINITIONS_H

#include "tucucore/operation.h"
#include "tucucore/definitions.h"


namespace Tucuxi {
namespace Core {


class Unit
{

};

/// \brief Default value for a drug (given either as a numerical value, or as an operation to obtain it).
/// A PopulationValue represents the value of a parameter needed for computing a drug's dosage, and is the default value
/// adopted when the corresponding patient information is missing.
class PopulationValue
{
public:
    /// \brief Create a population value from its components.
    /// \param _id Identifier of the value.
    /// \param _value Default value.
    /// \param _operation Operation associated with the parameter.
    PopulationValue(const std::string &_id, const Value _value, Operation *_operation) :
        m_id(_id),
        m_value(_value),
        m_operation(_operation)
    {}

    /// \brief Return the identifier of the value.
    /// \return Value's identifier.
    std::string getId() const { return m_id;}

    /// \brief Get the operation associated with the value.
    /// \return Shared pointer to the operation associated with the parameter.
    const Operation &getOperation() const { return *m_operation;}

    /// \brief Get the value.
    /// \return Returns the value.
    Value getValue() const { return m_value; }

protected:
    std::string m_id;
    Value m_value;
    std::unique_ptr<Operation const> m_operation;
};

template<typename DefinitionClass>
class IndividualValue
{
public:
    IndividualValue(const DefinitionClass &_definition) :
        m_definition(_definition)
    {    }

protected:

    const DefinitionClass &m_definition;
    const Operation& getOperation() const { return m_definition.getOperation(); }

};

}
}

#endif // DRUGDEFINITIONS_H
