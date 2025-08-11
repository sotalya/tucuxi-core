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


#ifndef VALIDDURATION_H
#define VALIDDURATION_H

#include <chrono>
#include <vector>

#include "tucucommon/duration.h"

#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"
#include "tucucore/validvalues.h"

namespace Tucuxi {
namespace Core {

class ValidDurations : public ValidValues
{
public:
    ValidDurations(TucuUnit _unit, std::unique_ptr<PopulationValue> _defaultValue);

    ~ValidDurations() override;

    Tucuxi::Common::Duration getDefaultDuration() const;

    virtual std::vector<Tucuxi::Common::Duration> getDurations() const;

    INVARIANTS(INVARIANT(
                       Invariants::INV_VALIDDURATIONS_0001,
                       (Common::UnitManager::isOfType<Common::UnitManager::UnitType::Time>(m_unit)),
                       "A duration does not have a corresponding unit");
               INVARIANT(
                       Invariants::INV_VALIDDURATIONS_0002,
                       (getDefaultDuration() >= Tucuxi::Common::Duration(std::chrono::hours(0))),
                       "A duration is negative");
               LAMBDA_INVARIANT(
                       Invariants::INV_VALIDDURATIONS_0003,
                       {
                           bool ok = true;
                           for (const auto& duration : getDurations()) {
                               ok &= duration >= Tucuxi::Common::Duration(std::chrono::hours(0));
                           }
                           return ok;
                       },
                       "There is a negative duration in the list of valid durations");)

protected:
    Tucuxi::Common::Duration valueToDuration(Value _value) const;
};

} // namespace Core
} // namespace Tucuxi


#endif // VALIDDURATION_H
