//@@license@@

#include "sample.h"


namespace Tucuxi {
namespace Core {


Sample::Sample(DateTime _date, AnalyteId _analyteId, Value _value, TucuUnit _unit, Value _weight)
    : m_date(_date), m_analyteID(_analyteId), m_value(_value), m_unit(_unit), m_weight(_weight)
{
}

Sample::~Sample() {}

DateTime Sample::getDate() const
{
    return m_date;
}

//std::string Sample::getSampleId() const
//{
//    return m_sampleID;
//}

TucuUnit Sample::getUnit() const
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

Value Sample::getWeight() const
{
    return m_weight;
}



///// Concentrations in every sample
//ConcentrationData::ConcentrationData(AnalyteId _analyteID,
//                                     Value _value,
//                                     Unit _unit) :
//    m_analyteID(_analyteID), m_value(_value), m_unit(_unit)
//{}



} // namespace Core
} // namespace Tucuxi
