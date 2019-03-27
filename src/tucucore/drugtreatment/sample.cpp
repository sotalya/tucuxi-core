/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "sample.h"


namespace Tucuxi {
namespace Core {


Sample::Sample(DateTime _date, std::string _analyteId, Value _value, Unit _unit) :
    m_analyteId(_analyteId), m_date(_date), m_value(_value), m_unit(_unit)
{

}

AnalyteId Sample::getAnalyteId() const
{
    return m_analyteId;
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

} // namespace Core
} // namespace Tucuxi
