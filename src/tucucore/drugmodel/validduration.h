/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDURATION_H
#define VALIDDURATION_H

#include <vector>

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

    virtual ~ValidDurations();

    Tucuxi::Common::Duration getDefaultDuration() const;

    virtual std::vector<Tucuxi::Common::Duration> getDurations() const;

    // TODO : Add invariants
    INVARIANTS()

protected:

    Tucuxi::Common::Duration valueToDuration(Value _value) const;

};

} // namespace Core
} // namespace Tucuxi


#endif // VALIDDURATION_H
