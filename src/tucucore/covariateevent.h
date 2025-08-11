/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_COVARIATE_H
#define TUCUXI_CORE_COVARIATE_H

#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operablegraphmanager.h"
#include "tucucore/operation.h"
#include "tucucore/timedevent.h"

#include "drugmodel/covariatedefinition.h"

namespace Tucuxi {
namespace Core {

/// \brief Model the change of a covariate.
class CovariateEvent : public IndividualValue<CovariateDefinition>, public TimedEvent, public Operable
{
public:
    /// \brief Remove the default constructor.
    CovariateEvent() = delete;
    /// \brief Create a change in a covariate given a reference to the desired covariate, a time, and the new value.
    /// \param _covariateDef Covariate definition that is changed.
    /// \param _date Time of change.
    /// \param _value New value of the covariate.
    CovariateEvent(const CovariateDefinition& _covariateDef, const DateTime& _date, Value _value)
        : IndividualValue(_covariateDef), TimedEvent(_date), Operable(_value)
    {
    }

    /// \brief Get the associated operation.
    /// \return Reference to the associated operation.
    Operation& getOperation() const override
    {
        return m_definition.getOperation();
    }

    /// \brief Perform the evaluation on the Operable, retrieving the inputs (and the dependencies) from the
    ///        OperableGraphManager.
    /// \param _graphMgr Reference to the graph manager where the Operable has to seek its inputs.
    /// \return True if the evaluation could be performed, false in case of errors.
    bool evaluate(const OperableGraphManager& _graphMgr) override;

    /// \brief Return the identifier of the covariate involved in the change.
    /// \return Identifier of covariate involved in the change.
    std::string getId() const
    {
        return m_definition.getId();
    }

    /// \brief Set the time the event happened.
    /// \param _time Time of the event.
    void setEventTime(const DateTime& _time)
    {
        m_time = _time;
    }
};

/// \brief List of covariate series (that is, changes).
typedef std::vector<CovariateEvent> CovariateSeries;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COVARIATEEVENT_H
