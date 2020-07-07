/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "sample.h"


namespace Tucuxi {
namespace Core {


Sample::Sample(std::string _sampleId, DateTime _date, AnalyteId _analyteId, Value _value, Unit _unit) :
    m_sampleID(_sampleId), m_date(_date), m_analyteID(_analyteId), m_value(_value), m_unit(_unit)
{}

DateTime Sample::getDate() const
{
    return m_date;
}

std::string Sample::getSampleId() const
{
    return m_sampleID;
}

Unit Sample::getUnit() const
{
    return m_unit;
}

AnalyteId Sample::getAnalyteID() const
{
    return m_analyteID;
}

Value Sample::getValue() const
{
    return m_value;
}



///// Concentrations in every sample
//ConcentrationData::ConcentrationData(AnalyteId _analyteID,
//                                     Value _value,
//                                     Unit _unit) :
//    m_analyteID(_analyteID), m_value(_value), m_unit(_unit)
//{}



} // namespace Core
} // namespace Tucuxi
