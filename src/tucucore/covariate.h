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

#include "drugmodel/covariatedefinition.h"
#include "drugtreatment/patientcovariate.h"

namespace Tucuxi {
namespace Core {

/// \brief Model the change of a covariate.
class CovariateEvent : public IndividualValue<CovariateDefinition>, public TimedEvent, public Operable
{
public:
    /// \brief Remove the default constructor.
    CovariateEvent() = delete;
    /// \brief Create a change in a covariate given a reference to the desired covariate, a time, and the new value.
    /// \param _covariateDef Covariate definition that is changed
    /// \param _date Time of change.
    /// \param _value New value of the covariate.
    CovariateEvent(const CovariateDefinition &_covariateDef, const DateTime &_date, Value _value)
        : IndividualValue(_covariateDef), TimedEvent(_date), Operable(_value),
          m_id(_covariateDef.getId()), m_value(_value)
    {}

    /// \brief Get the modified value of the covariate.
    /// \return Modified value of the covariate.
    Value getValue() const { return m_value; }

    /// \brief Return the identifier of the covariate involved in the change.
    /// \return Identifier of covariate involved in the change.
    std::string getId() const { return m_id; }

private:
    /// \brief Identifier of the covariate involved in the change
    std::string m_id;

    /// \brief New value of the covariate.
    Value m_value;
};

/// \brief List of covariate series (that is, changes).
typedef std::vector<CovariateEvent> CovariateSeries;

}
}

#endif // TUCUXI_CORE_COVARIATEEVENT_H
