#ifndef TUCUXI_MATH_DOSAGE_H
#define TUCUXI_MATH_DOSAGE_H

#include <vector>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class IntakeExtractor;
class IntakeSeries
{

};

#define DOSAGE_UTILS(className) \
    virtual void extract(IntakeExtractor *extractor, Date start, Date end, IntakeSeries &series); \
    friend IntakeExtractor;

class Dosage
{
    DOSAGE_UTILS(Dosage)

};

/// \brief A list of abstract dosages
typedef std::vector<Dosage*> DosageList;

class DosageUnbounded : public Dosage
{
    DOSAGE_UTILS(DosageUnbounded)

};

class DosageBounded : public Dosage
{
    DOSAGE_UTILS(DosageBounded)

};


/// \brief A list of bounded dosages
typedef std::vector<DosageBounded> DosageBoundedList;

class DosageLoop : public DosageUnbounded
{
    DOSAGE_UTILS(DosageLoop)

private:
    DosageBounded *m_dosage;
    unsigned int m_nbTimes;
};


class DosageRepeat : public DosageBounded
{
    DOSAGE_UTILS(DosageRepeat)

private:
    unsigned int m_nbTimes;
};

class DosageSequence : public DosageBounded
{
    DOSAGE_UTILS(DosageSequence)

private:
    DosageBoundedList m_dosages;
};

class SingleDose : public DosageBounded
{
    DOSAGE_UTILS(SingleDose)

private:
    Dose m_dose;
    // We need a type for this:
//    RouteOfAdministration m_routeOfAdministration;
};

class LastingDose : public SingleDose
{
    DOSAGE_UTILS(LastingDose)

private:
    Time m_interval;
};

class DailyDose : public SingleDose
{
    DOSAGE_UTILS(DailyDosage)

private:
    Time m_timeOfDay;
};

class WeeklyDose : public DailyDose
{
    DOSAGE_UTILS(WeeklyDose)

private:
    int m_dayOfWeek;
};

class DosageTimeRange
{
    friend IntakeExtractor;

private:
    DosageList m_dosages;
    Date m_startDate;
    Date m_endDate;
};

/// \brief A list of dosage time range
typedef std::vector<DosageTimeRange*> DosageTimeRangeList;

/// \ingroup TucuCore
/// \brief Represents a dosage history.
class DosageHistory
{
    friend IntakeExtractor;

public:
    /// \brief Constructor
    DosageHistory()
    {}

private:

    DosageTimeRangeList m_history;
};

}
}

#endif // TUCUXI_MATH_DOSAGE_H
