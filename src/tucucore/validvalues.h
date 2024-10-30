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


#ifndef VALIDVALUES_H
#define VALIDVALUES_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"


namespace Tucuxi {
namespace Core {


class IValidValues
{
public:
    virtual std::vector<Value> getValues() const = 0;

    virtual Value getToValue() const = 0;
    virtual Value getFromValue() const = 0;
    virtual Value getStepValue() const = 0;

    virtual ~IValidValues() {}
};


class ValidValues
{
public:
    ValidValues(Common::TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultValue);

    virtual ~ValidValues();

    TucuUnit getUnit() const;
    Value getDefaultValue() const;
    Value getStepValue() const;
    Value getToValue() const;
    Value getFromValue() const;

    virtual std::vector<Value> getValues() const;

    void addValues(std::unique_ptr<IValidValues> _values);

protected:
    TucuUnit m_unit;
    std::unique_ptr<PopulationValue> m_defaultValue;

    std::vector<std::unique_ptr<IValidValues> > m_valueSets;
};



class ValidValuesRange : public IValidValues
{
public:
    ValidValuesRange(
            std::unique_ptr<PopulationValue> _from,
            std::unique_ptr<PopulationValue> _to,
            std::unique_ptr<PopulationValue> _step);

    std::vector<Value> getValues() const override;

    Value getStepValue() const override;
    Value getToValue() const override;
    Value getFromValue() const override;

protected:
    std::unique_ptr<PopulationValue> m_from;
    std::unique_ptr<PopulationValue> m_to;
    std::unique_ptr<PopulationValue> m_step;
};


class ValidValuesFixed : public IValidValues
{

public:
    ValidValuesFixed();

    void addValue(Value _dose);
    std::vector<Value> getValues() const override;

    Value getStepValue() const override
    {
        return 0.0;
    } // Need to change this, only use for ValidValuesRange
    Value getToValue() const override
    {
        return 0.0;
    } // Need to change this, only use for ValidValuesRange
    Value getFromValue() const override
    {
        return 0.0;
    } // Need to change this, only use for ValidValuesRange¨

protected:
    std::vector<Value> m_values;
};




} // namespace Core
} // namespace Tucuxi

#endif // VALIDVALUES_H
