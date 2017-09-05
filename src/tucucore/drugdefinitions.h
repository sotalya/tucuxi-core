#ifndef DRUGDEFINITIONS_H
#define DRUGDEFINITIONS_H

#include "tucucore/operation.h"
#include "tucucore/definitions.h"


namespace Tucuxi {
namespace Core {


class PopulationValue
{
public:

    PopulationValue(std::string _id, Value _value, Operation *_operation) :
        m_id(_id),
        m_value(_value),
        m_operation(_operation)
    {}


    /// \brief Get the parameter value
    /// \return Returns the parameter value
    Value getValue() const { return m_value; }
    Operation *getOperation() const { return m_operation;}
    void setOperation(Operation *_operation) {m_operation = _operation;};
    std::string getId() const { return m_id;}

protected:
    std::string m_id;
    Value m_value;
    Operation *m_operation;
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
    Operation *getOperation() const { return m_definition.getOperation(); }

};

}
}

#endif // DRUGDEFINITIONS_H
