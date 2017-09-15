/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLE_H
#define TUCUXI_CORE_SAMPLE_H

#include <vector>

#include "tucucore/timedevent.h"
#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

class Sample
{
public:
    Sample(DateTime _date, Value _value, Unit _unit);

    DateTime getDate() const;
    Value getValue() const;
    Unit getUnit() const;

protected:
    DateTime m_date;
    Value m_value;
    Unit m_unit;
};

typedef std::vector<std::unique_ptr<Sample> > Samples;

}
}

#endif // TUCUXI_CORE_SAMPLE_H
