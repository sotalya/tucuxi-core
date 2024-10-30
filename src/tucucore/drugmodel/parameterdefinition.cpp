/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "parameterdefinition.h"

namespace Tucuxi {
namespace Core {

ParameterDefinition::ParameterDefinition(
        const std::string& _id, Value _value, std::unique_ptr<ParameterVariability> _variabilityType)
    : PopulationValue(_id, _value, nullptr), m_variability(std::move(_variabilityType))
{
}

ParameterDefinition::ParameterDefinition(const std::string& _id, Value _value) : PopulationValue(_id, _value, nullptr)
{
}


ParameterDefinition::ParameterDefinition(
        const std::string& _id, Value _value, ParameterVariabilityType _variabilityType)
    : PopulationValue(_id, _value, nullptr), m_variability(std::make_unique<ParameterVariability>(_variabilityType))
{
}

ParameterDefinition::ParameterDefinition(
        const std::string& _name,
        Value _value,
        std::unique_ptr<Operation> _operation,
        std::unique_ptr<ParameterVariability> _variabilityType)
    : PopulationValue(_name, _value, std::move(_operation)), m_variability(std::move(_variabilityType))
{
}

ParameterDefinition::ParameterDefinition(
        const std::string& _name,
        Value _value,
        std::unique_ptr<Operation> _operation,
        ParameterVariabilityType _variabilityType)
    : PopulationValue(_name, _value, std::move(_operation)),
      m_variability(std::make_unique<ParameterVariability>(_variabilityType))
{
}

ParameterDefinition::~ParameterDefinition() = default;

bool ParameterEvent::evaluate(const OperableGraphManager& _graphMgr)
{
    Operation& op = getOperation();

    // Collect inputs
    OperationInputList inputs = getInputs();

    for (auto& input : inputs) {
        double val = 0.0;
        bool rc = _graphMgr.getValue(input.getName(), val);
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

} // namespace Core
} // namespace Tucuxi
