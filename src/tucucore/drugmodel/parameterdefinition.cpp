/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "parameterdefinition.h"

namespace Tucuxi {
namespace Core {

ParameterDefinition::ParameterDefinition(const std::string _id, Value _value, std::unique_ptr<ParameterVariability> _variabilityType)
    : PopulationValue(_id, _value, nullptr),
      m_variability(std::move(_variabilityType))
{}

ParameterDefinition::ParameterDefinition(const std::string _id, Value _value)
    : PopulationValue(_id, _value, nullptr)
{}


ParameterDefinition::ParameterDefinition(const std::string _id, Value _value, ParameterVariabilityType _variabilityType)
    : PopulationValue(_id, _value, nullptr),
      m_variability(std::make_unique<ParameterVariability>(_variabilityType))
{}

ParameterDefinition::ParameterDefinition(const std::string _name, Value _value, Operation* _operation, std::unique_ptr<ParameterVariability> _variabilityType)
    : PopulationValue(_name, _value, _operation),
     m_variability(std::move(_variabilityType))
{}

ParameterDefinition::ParameterDefinition(std::string _name, Value _value, Operation* _operation, ParameterVariabilityType _variabilityType)
    : PopulationValue(_name, _value, _operation),
      m_variability(std::make_unique<ParameterVariability>(_variabilityType))
{

}

ParameterDefinition::~ParameterDefinition()
{}

bool ParameterEvent::evaluate(const OperableGraphManager &_graphMgr)
{
    Operation &op = getOperation();

    // Collect inputs
    OperationInputList inputs = getInputs();

    for (auto &input : inputs) {
        double val;
        bool rc;

        rc = _graphMgr.getValue(input.getName(), val);
        if (!rc) {
            return false;
        }

        rc = input.setValue(val);
        if (!rc) {
            return false;
        }

    }

    return op.evaluate(inputs, m_value);
}

}
}
