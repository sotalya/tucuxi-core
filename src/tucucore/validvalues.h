#ifndef VALIDVALUES_H
#define VALIDVALUES_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"


using Tucuxi::Common::Unit;

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
    ValidValues(Unit _unit, std::unique_ptr<PopulationValue> _defaultValue);

    virtual ~ValidValues();

    Unit getUnit() const;
    Value getDefaultValue() const;
    Value getStepValue() const;
    Value getToValue() const;
    Value getFromValue() const;

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

    Value getStepValue() const override {return 0.0;} // Need to change this, only use for ValidValuesRange
    Value getToValue() const override {return 0.0;} // Need to change this, only use for ValidValuesRange
    Value getFromValue() const override {return 0.0;} // Need to change this, only use for ValidValuesRange¨

protected:
    std::vector<Value> m_values;
};




} // namespace Core
} // namespace Tucuxi

#endif // VALIDVALUES_H
