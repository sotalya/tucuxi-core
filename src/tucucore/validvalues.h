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

    virtual ~IValidValues() {};
};


class ValidValues
{
public:
    ValidValues(Unit _unit, std::unique_ptr<PopulationValue> _defaultValue);

    virtual ~ValidValues();

    Unit getUnit() const;
    Value getDefaultValue() const;

    virtual std::vector<Value> getValues() const;

    void addValues(std::unique_ptr<IValidValues> _values);

protected:

    Unit m_unit;
    std::unique_ptr<PopulationValue> m_defaultValue;

    std::vector<std::unique_ptr<IValidValues> > m_valueSets;
};



class ValidValuesRange : public IValidValues
{
public:
    ValidValuesRange(
             std::unique_ptr<PopulationValue> _from,
             std::unique_ptr<PopulationValue> _to,
             std::unique_ptr<PopulationValue> _step
             );

    std::vector<Value> getValues() const override;

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

protected:
    std::vector<Value> m_values;
};




} // namespace Core
} // namespace Tucuxi

#endif // VALIDVALUES_H
