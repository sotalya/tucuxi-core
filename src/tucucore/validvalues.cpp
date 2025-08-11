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


#include <utility>

#include "validvalues.h"


namespace Tucuxi {
namespace Core {


ValidValues::ValidValues(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultValue)
    : m_unit(std::move(_unit)), m_defaultValue(std::move(_defaultValue))
{
}

ValidValues::~ValidValues() = default;

TucuUnit ValidValues::getUnit() const
{
    return m_unit;
}

Value ValidValues::getDefaultValue() const
{
    // TODO : manage the default dose with covariates
    return m_defaultValue->getValue();
}

Value ValidValues::getStepValue() const
{
    return std::move(m_valueSets[0])->getStepValue();
}

Value ValidValues::getToValue() const
{
    return std::move(m_valueSets[0])->getToValue();
}

Value ValidValues::getFromValue() const
{
    return std::move(m_valueSets[0])->getFromValue();
}

std::vector<Value> ValidValues::getValues() const
{
    std::vector<DoseValue> result;
    for (const auto& valueSet : m_valueSets) {
        std::vector<Value> tmp = valueSet->getValues();
        result.insert(std::end(result), std::begin(tmp), std::end(tmp));
    }

    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

void ValidValues::addValues(std::unique_ptr<IValidValues> _values)
{
    m_valueSets.push_back(std::move(_values));
}


ValidValuesRange::ValidValuesRange(
        std::unique_ptr<PopulationValue> _from,
        std::unique_ptr<PopulationValue> _to,
        std::unique_ptr<PopulationValue> _step)
    : m_from(std::move(_from)), m_to(std::move(_to)), m_step(std::move(_step))
{
}

std::vector<Value> ValidValuesRange::getValues() const
{
    // TODO : manage the covariates for generating the doses

    Value currentDose = m_from->getValue();
    std::vector<Value> result;

    while (currentDose <= m_to->getValue()) {
        result.push_back(currentDose);
        currentDose += m_step->getValue();
    }
    return result;
}

Value ValidValuesRange::getToValue() const
{
    return m_to->getValue();
}

Value ValidValuesRange::getFromValue() const
{
    return m_from->getValue();
}

Value ValidValuesRange::getStepValue() const
{
    return m_step->getValue();
}


ValidValuesFixed::ValidValuesFixed() = default;

void ValidValuesFixed::addValue(Value _dose)
{
    m_values.push_back(_dose);
}

std::vector<Value> ValidValuesFixed::getValues() const
{
    return m_values;
}

} // namespace Core
} // namespace Tucuxi
