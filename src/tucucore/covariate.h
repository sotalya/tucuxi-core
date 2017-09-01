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

namespace Tucuxi {
namespace Core {

enum class CovariateType {
    Fixed = 0,          // Use the default value or the one specified in the drug treatment if existing
    Discret,            // Use changes of value as defined in the drug treatment
    Interpolated,       // Discret values are interpolated
    Operable            // The value depends on another covariate
};

///
/// \brief The CovariateDefinition class
/// The definition of a covariate
class CovariateDefinition
{

};

class Covariate
{

private:
    std::string m_covariateId;
    CovariateType m_type;
    Value m_defaultValue;
    // Unit ...

    Tucuxi::Common::Duration m_refreshPeriod;   // Only in the case of CovariateType::Interpolated
    Operation *m_operation; // Operation depending on another covariate in case Operable
};

typedef std::vector<Covariate> Covariates;


class CovariateEvent : public TimedEvent, Operable
{
public:
    CovariateEvent() = delete;
    CovariateEvent(const Covariate& _covariate, const DateTime& _date, Value _value)
        : TimedEvent(_date), Operable(_value), m_definition(_covariate), m_value(_value)
    {}

    Value getValue() { return m_value; }

private:
    const Covariate &m_definition;
    // Should be removed, embedded in Operable
    Value m_value;
};

typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
