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

    std::string getComputationId() { return m_computationId;}

    void setComputationId(const std::string &id) { m_computationId = id;}

private:
    std::string m_computationId;
    Value m_value;
    Operation *m_operation; // Operation depending on another covariate in case Operable
};

typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
