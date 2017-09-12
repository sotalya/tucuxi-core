#ifndef DRUGDEFINITIONS_H
#define DRUGDEFINITIONS_H

#include "tucucore/operation.h"
#include "tucucore/definitions.h"


namespace Tucuxi {
namespace Core {


class Unit
{

};

class PopulationValue
{
public:

    PopulationValue(std::string _id, Value _value, std::shared_ptr<const Operation> _operation) :
        m_id(_id),
        m_value(_value),
        m_operation(_operation)
    {}
/*
    PopulationValue(PopulationValue&& other) :
        m_operation(std::move(other.m_operation))
    {
        m_id = other.m_id;
        m_value = other.m_value;
//        m_operation = std::move(other.m_operation);
    }

    PopulationValue(const PopulationValue& other) :
        m_operation(std::move(other.m_operation))
    {
        m_id = other.m_id;
        m_value = other.m_value;
//        m_operation = std::move(other.m_operation);
    }
*/


    /// \brief Get the parameter value
    /// \return Returns the parameter value
    Value getValue() const { return m_value; }
    std::shared_ptr<const Operation> getOperation() const { return m_operation;}
//    const Operation &getOperation() const { return *m_operation;}
//    void setOperation(Operation *_operation) {m_operation = _operation;};
    std::string getId() const { return m_id;}

protected:
    std::string m_id;
    Value m_value;
    std::shared_ptr<const Operation> m_operation;
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
