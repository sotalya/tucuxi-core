/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDURATION_H
#define VALIDDURATION_H

#include <vector>
#include <chrono>

#include "tucucommon/duration.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/validvalues.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class ValidDurations : public ValidValues
{
public:
    ValidDurations(Unit _unit, std::unique_ptr<PopulationValue> _defaultValue);

    ~ValidDurations() override;

    Tucuxi::Common::Duration getDefaultDuration() const;

    virtual std::vector<Tucuxi::Common::Duration> getDurations() const;

    INVARIANTS(
            INVARIANT(Invariants::INV_VALIDDURATIONS_0001, (m_unit.isTime()), "A duration does not have a corresponding unit")
            INVARIANT(Invariants::INV_VALIDDURATIONS_0002, (getDefaultDuration() >= Tucuxi::Common::Duration(std::chrono::hours(0))), "A duration is negative")
            LAMBDA_INVARIANT(Invariants::INV_VALIDDURATIONS_0003, {bool ok = true;for(const auto &duration : getDurations()) {ok &= duration >= Tucuxi::Common::Duration(std::chrono::hours(0));} return ok;}, "There is a negative duration in the list of valid durations")
            )

protected:

    Tucuxi::Common::Duration valueToDuration(Value _value) const;

};

} // namespace Core
} // namespace Tucuxi


#endif // VALIDDURATION_H
