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
    Fixed = 0,          /// Use the default value or the one specified in the drug treatment if existing
    Discret,            /// Use changes of value as defined in the drug treatment
    Interpolated,       /// Discret values are interpolated
    Operable            /// The value depends on another covariate
};

enum class DataType {
    Bool = 0,
    Int,
    Double
};

///
/// \brief The CovariateDefinition class
/// The definition of a covariate
class CovariateDefinition : public PopulationValue
{
public:

    CovariateDefinition(const std::string _id, Value _value, Operation* _operation, CovariateType _type) :
        PopulationValue(_id, _value, _operation), m_type(_type) {}

    CovariateType m_type;
    DataType m_dataType;
    Unit m_unit;
    Tucuxi::Common::Duration m_refreshPeriod;   // Only in the case of CovariateType::Interpolated

};

typedef std::vector<std::unique_ptr<CovariateDefinition> > CovariateDefinitions;


class PatientVariate : public TimedEvent
{
    Value m_value;
    std::string m_id;
    DataType m_dataType;
    Unit m_unit;
};

typedef std::vector<std::unique_ptr<PatientVariate> > PatientVariates;
/*
class Covariate : public IndividualValue<CovariateDefinition>
{

private:
    // Unit ...

};
*/

//typedef std::vector<Covariate> Covariates;


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
