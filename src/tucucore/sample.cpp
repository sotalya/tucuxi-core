/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "sample.h"


namespace Tucuxi {
namespace Core {


Sample::Sample(DateTime _date, Value _value, Unit _unit) :
    m_date(_date), m_value(_value), m_unit(_unit)
{

}

DateTime Sample::getDate() const
{
    return m_date;
}

Value Sample::getValue() const
{
    return m_value;
}

Unit Sample::getUnit() const
{
    return m_unit;
}

}
}
