/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDOSE_H
#define VALIDDOSE_H

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

class ValidDoses
{
public:
    ValidDoses(Unit _unit, Value _defaultDose);

    virtual ~ValidDoses() {};

    Unit getUnit() const;
    Value getDefaultDose() const;

    virtual std::vector<Value> getDoses() const = 0;

protected:

    Unit m_unit;
    Value m_defaultDose;
};

class AnyDoses : public ValidDoses
{
public:
    AnyDoses(Unit _unit, Value _defaultDose, Value _from, Value _to, Value _step);

    std::vector<Value> getDoses() const override;

protected:
    Value m_from;
    Value m_to;
    Value m_step;
};


class SpecificDoses : public ValidDoses
{

public:

    SpecificDoses(Unit _unit, Value _defaultDose);

    std::vector<Value> getDoses() const override;

protected:
    std::vector<Value> m_doses;
};

}
}


#endif // VALIDDOSE_H
