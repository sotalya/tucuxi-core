/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef VALIDDURATION_H
#define VALIDDURATION_H

#include <vector>

#include "tucucommon/duration.h"

namespace Tucuxi {
namespace Core {

class ValidDurations
{
public:
    ValidDurations(Tucuxi::Common::Duration _defaultDuration);

    virtual ~ValidDurations() {};


    virtual std::vector<Tucuxi::Common::Duration> getDurations() const = 0;
    void setDefaultDuration(Tucuxi::Common::Duration _duration);
    Tucuxi::Common::Duration getDefaultDuration() const;

protected:
    Tucuxi::Common::Duration m_defaultDuration;

};

class AnyDurations : public ValidDurations
{
public:
    AnyDurations(
            Tucuxi::Common::Duration _default,
            Tucuxi::Common::Duration _from,
            Tucuxi::Common::Duration _to,
            Tucuxi::Common::Duration _step);

    virtual std::vector<Tucuxi::Common::Duration> getDurations() const;
protected:
    Tucuxi::Common::Duration m_from;
    Tucuxi::Common::Duration m_to;
    Tucuxi::Common::Duration m_step;
};

class SpecificDurations : public ValidDurations
{

public:
    SpecificDurations(Tucuxi::Common::Duration _default);

    virtual std::vector<Tucuxi::Common::Duration> getDurations() const;

    void addDuration(Tucuxi::Common::Duration _duration);

//    SpecificDurations() {};

protected:

    std::vector<Tucuxi::Common::Duration> m_durations;
};

} // namespace Core
} // namespace Tucuxi


#endif // VALIDDURATION_H
