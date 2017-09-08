/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATE_H
#define TUCUXI_CORE_COVARIATE_H

#include <vector>

#include "tucucommon/duration.h"

#include "tucucore/definitions.h"
#include "tucucore/timedevent.h"
#include "tucucore/operation.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

enum class CovariateType {
    Standard = 0,
    AgeInYears,
    AgeInDays,
    AgeInMonths
//    Fixed = 0,          /// Use the default value or the one specified in the drug treatment if existing
//    Discret,            /// Use changes of value as defined in the drug treatment
//    Interpolated,       /// Discret values are interpolated
//    Operable            /// The value depends on another covariate
};

enum class DataType {
    Int = 0,
    Double,
    Bool,
    Date
};

enum class InterpolationType
{
    Direct = 0,
    Linear,
    Sigmoid,
    Tanh
};

///
/// \brief The CovariateDefinition class
/// The definition of a covariate
class CovariateDefinition : public PopulationValue
{
public:

    // TODO : Make variables protected and write accessors

    CovariateDefinition(const std::string _id, Value _value, Operation* _operation, CovariateType _type) :
        PopulationValue(_id, _value, _operation), m_type(_type) {}

    CovariateType m_type;
    DataType m_dataType;
    InterpolationType m_interpolationType;
    Unit m_unit;
    Tucuxi::Common::Duration m_refreshPeriod;   // Only in the case of CovariateType::Interpolated


};

typedef std::vector<std::unique_ptr<CovariateDefinition> > CovariateDefinitions;


class PatientCovariate : public TimedEvent
{
    // TODO : Make variables protected and write accessors

    std::string m_value;
    std::string m_id;
    DataType m_dataType;
    Unit m_unit;
};

typedef std::vector<std::unique_ptr<PatientCovariate> > PatientVariates;


class CovariateEvent :  public IndividualValue<CovariateDefinition>, TimedEvent, Operable
{
public:
    CovariateEvent() = delete;
    CovariateEvent(const CovariateDefinition& _covariateDef, const DateTime& _date, Value _value)
        : IndividualValue(_covariateDef), TimedEvent(_date), Operable(_value), m_value(_value)
    {}

    Value getValue() { return m_value; }

private:
    Value m_value;

};

typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
