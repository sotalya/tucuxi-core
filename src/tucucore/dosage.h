#ifndef TUCUXI_MATH_DOSAGE_H
#define TUCUXI_MATH_DOSAGE_H

#include <memory>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"

#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/timeofday.h"

using Tucuxi::Common::DateTime;
using Tucuxi::Common::Duration;
using Tucuxi::Common::TimeOfDay;

namespace Tucuxi {
namespace Core {

class IntakeExtractor;

/// \brief An intake series is list of intake events.
/// This vector could be replaced in the future with a class that offers more capabilities (for instance, it could
/// incorporate the time interval spanned by the series). However, it will have to expose the same capabilities as the
/// underlying std::vector, thus the transition will be painless.
typedef std::vector<IntakeEvent> IntakeSeries;

/// \brief Implement the extract and clone operations for Dosage subclasses.
#define DOSAGE_UTILS(BaseClassName, ClassName) \
    friend IntakeExtractor; \
    virtual int extract(IntakeExtractor &_extractor, const DateTime &_start, const DateTime &_end, IntakeSeries &_series); \
    virtual std::unique_ptr<BaseClassName> clone() const \
    { \
        return std::unique_ptr<BaseClassName>(new ClassName(*this)); \
    } \
    virtual std::unique_ptr<Dosage> cloneDosage() const \
    { \
        return clone(); \
    }


/// \ingroup TucuCore
/// \brief Abstract class at the base of the dosage class hierarchy.
/// Dosages are implemented as either bounded or unbounded dosages.
class Dosage
{
public:
    /// \brief Extract drugs' intake in a given time interval (visitor pattern).
    /// The intakes will be added to the series passed as a sequence
    /// \param _extractor Intake extractor.
    /// \param _start Start time/date for the considered interval.
    /// \param _end End time/date for the considered interval, could be unset.
    /// \param _series Returned series of intake in the considered interval.
    /// \return number of intakes in the given interval, -1 in case of error.
    ///
    /// \pre _start IS NOT unset
    /// \pre _end IS unset OR _end > _start
    /// \post _series[OUT] = _series[IN] + extracted_intakes
    /// (it would have been easier to simply empty the input _series, but this guarantees an uniform behavior across the
    /// whole set of calls)
    /// \post FORALL intake IN extracted_intakes, intake.time IN [_start, _end)
    /// \see IntakeExtractor::extract()
    virtual int extract(IntakeExtractor &_extractor, const DateTime &_start, const DateTime &_end, IntakeSeries &_series) = 0;

    /// \brief Return a pointer to a clone of the correct subclass.
    /// \return Pointer to a new object of subclass' type.
    virtual std::unique_ptr<Dosage> cloneDosage() const = 0;
};

/// \brief A list of abstract dosages
typedef std::vector<std::unique_ptr<Dosage>> DosageList;

/// \ingroup TucuCore
/// \brief Pure virtual class upon which dosage loops are based.
class DosageUnbounded : public Dosage
{
public:
    /// \brief Return a pointer to a clone of the correct subclass.
    /// \return Pointer to a new object of subclass' type.
    virtual std::unique_ptr<DosageUnbounded> clone() const = 0;
};

/// \ingroup TucuCore
/// \brief Pure virtual class upon which all implemented dosages are based.
class DosageBounded : public Dosage
{
public:
    friend IntakeExtractor;

    // We must declare it, since it is used when invoking extract() in the different subclasses (we want to be able to
    // make the call, even if this function never gets called).
    virtual int extract(IntakeExtractor &_extractor, const DateTime &_start, const DateTime &_end, IntakeSeries &_series) = 0;

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the interval.
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const = 0;

    /// \brief Get the time step between two bounded dosages.
    /// This time step could represent the time interval between two weekly doses (in that case, a week), or the time
    /// taken by a more complex sequence (represented in this case by a DosageSequence), ...
    /// \return Time step size.
    virtual Duration getTimeStep() const = 0;

    /// \brief Return a pointer to a clone of the correct subclass.
    /// \return Pointer to a new object of subclass' type.
    virtual std::unique_ptr<DosageBounded> clone() const = 0;
};


/// \brief A list of bounded dosages
typedef std::vector<std::unique_ptr<DosageBounded>> DosageBoundedList;

/// \ingroup TucuCore
/// \brief List of bounded dosages that are repeated in time.
/// The bounds are imposed by the time range embedding the dosage loop. This class is created to ensure that no
/// unbounded history is part of another dosage loop (creating an infinite loop).
class DosageLoop : public DosageUnbounded
{
public:
    /// \brief Copy-construct a dosage loop.
    /// \param _other Dosage loop to clone.
    DosageLoop(const DosageLoop &_other) : DosageUnbounded(_other)
    {
        m_dosage = _other.m_dosage->clone();
    }

    DOSAGE_UTILS(DosageUnbounded, DosageLoop);

private:
    /// \brief Dosage that is repeated in the unbounded interval.
    std::unique_ptr<DosageBounded> m_dosage;
};

/// \ingroup TucuCore
/// \brief Dosage that is administered a given number of times.
class DosageRepeat : public DosageBounded
{
public:
    /// \brief Construct a repeated dosage starting from a dosage and the number of times it is repeated.
    /// \param _dosage Dosage to repeat.
    /// \param _nbTimes Number of times the dosage has to be repeated.
    DosageRepeat(const DosageBounded &_dosage,
                 const unsigned int _nbTimes)
    {
        m_dosage = _dosage.clone();
        m_nbTimes = _nbTimes;
    }

    /// \brief Copy-construct a dosage repetition.
    /// \param _other Dosage repetition to clone.
    DosageRepeat(const DosageRepeat &_other) : DosageBounded(_other)
    {
        m_dosage = _other.m_dosage->clone();
    }

    DOSAGE_UTILS(DosageBounded, DosageRepeat);

    /// \brief Get the time step between two bounded dosages.
    /// \return Time step of the whole repeated sequence.
    virtual Duration getTimeStep() const
    {
        return m_dosage->getTimeStep() * m_nbTimes;
    }

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the repeated sequence.
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const
    {
        return m_dosage->getFirstIntakeInterval(_intervalStart);
    }

private:
    /// \brief Dosage that is repeated.
    std::unique_ptr<DosageBounded> m_dosage;

    /// \brief Number of times a dosage is repeated.
    unsigned int m_nbTimes;
};

/// \ingroup TucuCore
/// \brief Ordered sequence of dosages that is administered in a bounded interval.
class DosageSequence : public DosageBounded
{
public:
    /// \brief Copy-construct a dosage sequence.
    /// \param _other Dosage sequence to clone.
    DosageSequence(const DosageSequence &_other) : DosageBounded(_other)
    {
        for (auto&& dosage: _other.m_dosages) {
            m_dosages.emplace_back(dosage->clone());
        }
    }

    DOSAGE_UTILS(DosageBounded, DosageSequence);

    /// \brief Get the time step between two bounded dosages.
    /// \return Time step of the whole repeated sequence.
    virtual Duration getTimeStep() const
    {
        Duration totalDuration;
        for (auto&& dosage : m_dosages) {
            totalDuration += dosage->getTimeStep();
        }
        return totalDuration;
    }

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the first element of the sequence (it is ordered).
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const
    {
        return (m_dosages.at(0))->getFirstIntakeInterval(_intervalStart);
    }

private:
    /// \brief Sequence of bounded dosages that is administered.
    DosageBoundedList m_dosages;
};

/// \ingroup TucuCore
/// \brief Abstract class specifying a specific intake.
class SingleDose : public DosageBounded
{
public:
    /// \brief Initialize a single dose from its components.
    /// \param _dose Quantity of drug administered.
    /// \param _route Route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    SingleDose(const Dose &_dose, const RouteOfAdministration &_routeOfAdministration,
               const Duration &_infusionTime)
    {
        m_dose = _dose;
        m_routeOfAdministration = _routeOfAdministration;
        m_infusionTime = _infusionTime;
    }

    /// \brief Empty destructor, used to make the class abstract (and thus force the instantiation of subclasses).
    virtual ~SingleDose() = 0;

protected:
    /// \brief Administered dose.
    Dose m_dose;
    /// \brief Route of administration.
    RouteOfAdministration m_routeOfAdministration;
    /// \brief Duration in case of an infusion.
    Duration m_infusionTime;
};

/// \ingroup TucuCore
/// \brief Dose supposed to last for a certain, specified time interval.
/// When used in a DosageLoop, a LastingDose object allows to represent a dosage history with fixed intervals.
class LastingDose : public SingleDose
{
public:
    /// \brief Initialize a lasting dose from its components.
    /// \param _dose Quantity of drug administered.
    /// \param _route Route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _interval Interval between two doses.
    LastingDose(const Dose &_dose, const RouteOfAdministration &_routeOfAdministration,
                const Duration &_infusionTime, const Duration &_interval)
        : SingleDose(_dose, _routeOfAdministration, _infusionTime)
    {
        m_interval = _interval;
    }

    DOSAGE_UTILS(DosageBounded, LastingDose);

    /// \brief Return the increment between two successive intakes.
    /// \return Interval between two lasting doses.
    virtual Duration getTimeStep() const
    {
        return m_interval;
    }

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the interval.
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const
    {
        return _intervalStart;
    }

private:
    /// \brief Interval between two doses.
    Duration m_interval;
};

/// \ingroup TucuCore
/// \brief Dose taken every day at the same time.
class DailyDose : public SingleDose
{
public:
    /// \brief Create a class representing a weekly dose.
    /// \param _dose Quantity of drug administered.
    /// \param _route Route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _timeOfDay Time of the day when the dose is administered.
    DailyDose(const Dose &_dose,
              const RouteOfAdministration &_routeOfAdministration,
              const Duration &_infusionTime,
              const TimeOfDay &_timeOfDay)
        : SingleDose(_dose, _routeOfAdministration, _infusionTime)
    {
        m_timeOfDay = _timeOfDay;
    }

    DOSAGE_UTILS(DosageBounded, DailyDose);

    /// \brief Return the increment between two successive intakes.
    /// \return Interval between two daily doses.
    virtual Duration getTimeStep() const
    {
        return Duration(std::chrono::hours(24));
    }

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the interval.
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const
    {
        DateTime intakeTime = _intervalStart;
        // The hour is fixed
        intakeTime.setTimeOfDay(m_timeOfDay);

        // If the beginning of the interval is past the intake time, then move to the following day
        if (_intervalStart.getTimeOfDay() > m_timeOfDay) {
            intakeTime += std::chrono::hours(24);
        }

        return intakeTime;
    }

protected:
    /// \brief Time of the day when the dose is administered.
    TimeOfDay m_timeOfDay;
};

/// \ingroup TucuCore
/// \brief Dose taken every week on a specific day of the week.
class WeeklyDose : public DailyDose
{
public:
    /// \brief Create a class representing a weekly dose.
    /// \param _dose Quantity of drug administered.
    /// \param _route Route of administration.
    /// \param _infusionTime Duration in case of an infusion.
    /// \param _timeOfDay Time of the day when the dose is administered.
    /// \param _dayOfWeek Day of the week the dose has to be administered.
    WeeklyDose(const Dose &_dose,
               const RouteOfAdministration &_routeOfAdministration,
               const Duration &_infusionTime,
               const TimeOfDay &_timeOfDay,
               const DayOfWeek &_dayOfWeek)
        : DailyDose(_dose, _routeOfAdministration, _infusionTime, _timeOfDay)
    {
        m_dayOfWeek = _dayOfWeek;
    }

    DOSAGE_UTILS(DosageBounded, WeeklyDose);

    /// \brief Return the increment between two successive intakes.
    /// \return Interval between two weekly doses.
    virtual Duration getTimeStep() const
    {
        return Duration(std::chrono::hours(7 * 24));
    }

    /// \brief Return the instant of the first intake in the given interval.
    /// \param _intervalStart Starting point of the interval.
    /// \return Time of the first intake.
    virtual DateTime getFirstIntakeInterval(const DateTime &_intervalStart) const
    {
        // Really ugly cast required by the library to extract the day of week from the date
        const int numStartDayOfWeek = (unsigned)(DayOfWeek(_intervalStart.getDate()));
        const int numPlannedDayOfWeek = (unsigned)m_dayOfWeek;
        int dayDiff = numPlannedDayOfWeek - numStartDayOfWeek;

        if (dayDiff < 0) {
            // The intake was supposed to be some days before, we have to advance by the days that are missing to
            // encompass a full week time
            dayDiff = 7 + dayDiff;

        } else if (dayDiff == 0 && _intervalStart.getTimeOfDay() > m_timeOfDay) {
            // We just missed the intake, we have to advance by one week
            dayDiff = 7;
        }

        DateTime intakeTime = _intervalStart;
        intakeTime += Duration(std::chrono::hours(24 * dayDiff));
        intakeTime.setTimeOfDay(m_timeOfDay);

        return intakeTime;
    }

private:
    /// \brief Day of the week the dose has to be administered.
    DayOfWeek m_dayOfWeek;
};

class DosageTimeRange;
/// \brief A list of dosage time range
typedef std::vector<std::unique_ptr<DosageTimeRange>> DosageTimeRangeList;

/// \ingroup TucuCore
/// \brief Doses administered in a given time interval (which might have no upper bound).
class DosageTimeRange
{
    friend IntakeExtractor;

public:
    /// \brief Initialize an open-ended list of doses (the end date is not specified).
    /// \param _startDate Interval's starting date.
    /// \pre !_startDate.isUndefined()
    /// \post m_startDate == _startDate
    /// \post _endDate.isUndefined()
    DosageTimeRange(const DateTime &_startDate) : m_startDate(_startDate)
    {
        if (m_startDate.isUndefined()) {
            throw std::invalid_argument("Undefined start date for a time range specified.");
        }
        m_endDate.reset();
    }

    /// \brief Copy-construct a time range.
    /// \param _other Time range to clone.
    DosageTimeRange(const DosageTimeRange &_other) : m_startDate(_other.m_startDate), m_endDate(_other.m_endDate)
    {
        for (auto&& dosage: _other.m_dosages) {
            m_dosages.emplace_back(dosage->cloneDosage());
        }
    }

    /// \brief Add a dosage to the time range list of dosages.
    /// \param _dosage Dosage to add
    void addDosage(const Dosage &_dosage)
    {
        m_dosages.emplace_back(_dosage.cloneDosage());
    }

    /// \brief Initialize an list of doses (the end date is specified, though it could be unset).
    /// \param _startDate Interval's starting date.
    /// \param _endDate Interval's ending date.
    /// \pre !_startDate.isUndefined()
    /// \post m_startDate == _startDate
    /// \post m_endDate == _endDate
    DosageTimeRange(const DateTime& _startDate, const DateTime& _endDate) : m_startDate(_startDate), m_endDate(_endDate)
    {
        if (m_startDate.isUndefined()) {
            throw std::invalid_argument("Undefined start date for a time range specified.");
        }
        if (!m_endDate.isUndefined() && m_startDate > m_endDate) {
            throw std::invalid_argument("The start date for a time range has to precede the end date.");
        }
    }

    /// \brief Check a time range against a list of time ranges for overlaps.
    /// \param _timeRange Time range to check.
    /// \param _timeRangeList List of time ranges to compare against.
    /// \return true if the given time range overlaps with at least one time range of the list, false otherwise.
    friend bool timeRangesOverlap(const DosageTimeRange &_timeRange, const DosageTimeRangeList &_timeRangeList);

    /// \brief Check if two time ranges overlap.
    /// \param _first First time range to check.
    /// \param _second Second time range to check.
    /// \return true if the two time ranges overlap, false otherwise.
    friend bool timeRangesOverlap(const DosageTimeRange &_first, const DosageTimeRange &_second);

private:
    /// List of doses administered in the interval.
    DosageList m_dosages;
    /// Start date of the interval. Must be a valid date.
    DateTime m_startDate;
    /// End date of the interval. Can be unset.
    DateTime m_endDate;
};

/// \ingroup TucuCore
/// \brief Represents a dosage history.
/// A dosage history represents every intake of a drug treatment, dealing with multiple intake scenarios.
class DosageHistory
{
    friend IntakeExtractor;

public:
    /// \brief Create a new history starting from a dosage time range list.
    /// \param _timeRangeList List of time ranges to import.
    DosageHistory(const DosageTimeRangeList &_timeRangeList)
    {
        for (auto&& timeRange: _timeRangeList) {
            if (timeRangesOverlap(*timeRange, m_history)) {
                throw std::runtime_error("Time range overlaps with previously-inserted one.");
            }
            addTimeRange(*timeRange);
        }
    }

    /// \brief Add a time range to the history.
    /// \param _timeRange Time range to add.
    void addTimeRange(const DosageTimeRange &_timeRange)
    {
        m_history.emplace_back(new DosageTimeRange(_timeRange));
    }

private:
    /// Usage history, expressed as a list of non-overlapping time intervals. The last one might have no end date (if
    /// the interval is open, that is, up to present date).
    DosageTimeRangeList m_history;
};

}
}

#endif // TUCUXI_MATH_DOSAGE_H
