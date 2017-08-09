/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_COVARIATE_H
#define TUCUXI_CORE_COVARIATE_H

#include <vector>

#include "tucucommon/duration.h"

#include "tucucore/definitions.h"
#include "tucucore/timedevent.h"

namespace Tucuxi {
namespace Core {

enum class CovariateType {
    Fixed = 0,          // Use the default value or the one specified in the drug treatment if existing
    Discret,            // Use changes of value as defined in the drug treatment
    Interpolated,       // Discret values are interpolated
    Operable            // The value depends on another covariate
};

class Covariate
{

private:
    CovariateType m_type;
    Tucuxi::Common::Duration m_refreshPeriod;   // Only in the case of CovariateType::Interpolated 
};

typedef std::vector<Covariate> Covariates;


class CovariateEvent : public TimedEvent
{
public:
    CovariateEvent() = delete;
    CovariateEvent(const DateTime& _date, Value _value)
        : TimedEvent(_date), m_value(_value)
    {}

    Value getValue() { return m_value; }

private:
    Value m_value;
};

typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H