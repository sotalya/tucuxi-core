#ifndef TUCUXI_CORE_INTAKEEXTRACTOR_H
#define TUCUXI_CORE_INTAKEEXTRACTOR_H

#include <algorithm>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/intakeevent.h"

#include "tucucommon/datetime.h"

using Tucuxi::Common::DateTime;

namespace Tucuxi {
namespace Core {

class IntakeExtractor
{
    friend DosageBounded;
    friend DosageLoop;
    friend DosageRepeat;
    friend DosageSequence;
    friend ParallelDosageSequence;
    friend SingleDose;
    friend LastingDose;
    friend DailyDose;
    friend WeeklyDose;

public:

    /// \ingroup TucuCore
    /// \brief Iterate over the usage history in a given period, extracting a list of intakes.
    ///
    /// \param _dosageHistory Dosage history.
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
    static int extract(std::unique_ptr<const DosageHistory> &_dosageHistory, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

private:

    /// \ingroup TucuCore
    /// \brief Iterate over the specified time range and extract the list of intakes.
    /// The time range could be smaller than the time range contained in the data structure, therefore the smallest
    /// subset has to be sought.
    ///
    /// \param _timeRange Specified time range.
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
    int extract(const DosageTimeRange &_timeRange, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Abstract function that is needed to properly represent the hierarchy.
    ///
    /// \param _dosageBounded Bounded dosage.
    /// \param _start Start time/date for the considered interval.
    /// \param _end End time/date for the considered interval, could be unset.
    /// \param _series Returned series of intake in the considered interval.
    /// \return number of intakes in the given interval, -1 in case of error.
    int extract(const DosageBounded &, const DateTime &, const DateTime &, IntakeSeries &);

    /// \ingroup TucuCore
    /// \brief Iterate over a loop of dosages and extract the list of intakes.
    /// For a dosage loop the bounds are given by the embedding time range. This means that we have to repeat the
    /// bounded dosage pointed by the class until the bounds of the time range are reached.
    ///
    /// \param _dosageLoop Dosage loop.
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
    int extract(const DosageLoop &_dosageLoop, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Iterate over a list of repeated dosages and extract the list of intakes.
    ///
    /// \param _dosageRepeat Repeated dosages.
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
    int extract(const DosageRepeat &_dosageRepeat, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Iterate over an ordered sequence of dosages and extract the list of intakes.
    ///
    /// \param _dosageSequence Ordered sequence of dosages.
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
    int extract(const DosageSequence &_dosageSequence, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Iterate over an ordered sequence of parallel dosages and extract the list of intakes.
    ///
    /// \param _parallelDosageSequence Ordered sequence of parallel dosages.
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
    int extract(const ParallelDosageSequence &_parallelDosageSequence, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Extract a dose supposed to last for a certain time and add it to a list of intakes.
    /// The operation is performed only if the intake sits inside the specified time bounds.
    ///
    /// \param _dosage Dose to extract.
    /// \param _start Start time/date for the considered interval.
    /// \param _end End time/date for the considered interval, could be unset.
    /// \param _series Returned series of intake in the considered interval.
    /// \return 1 if the intake lies in the specified interval, 0 otherwise.
    ///
    /// \pre _start IS NOT unset
    /// \pre _end IS unset OR _end > _start
    /// \post if (inside time bounds) { _series[OUT] = _series[IN] + intake } else { _series[OUT] = _series[IN] }
    /// \post FORALL intake IN extracted_intakes, intake.time IN [_start, _end)
    int extract(const LastingDose &_dosage, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Extract a daily dose and add it to a list of intakes.
    /// The operation is performed only if the intake sits inside the specified time bounds.
    ///
    /// \param _dosage Dose to extract.
    /// \param _start Start time/date for the considered interval.
    /// \param _end End time/date for the considered interval, could be unset.
    /// \param _series Returned series of intake in the considered interval.
    /// \return 1 if the intake lies in the specified interval, 0 otherwise.
    ///
    /// \pre _start IS NOT unset
    /// \pre _end IS unset OR _end > _start
    /// \post if (inside time bounds) { _series[OUT] = _series[IN] + intake } else { _series[OUT] = _series[IN] }
    /// \post FORALL intake IN extracted_intakes, intake.time IN [_start, _end)
    int extract(const DailyDose &_dosage, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);

    /// \ingroup TucuCore
    /// \brief Extract a weekly dose and add it to a list of intakes.
    /// The operation is performed only if the intake sits inside the specified time bounds.
    ///
    /// \param _dosage Dose to extract.
    /// \param _start Start time/date for the considered interval.
    /// \param _end End time/date for the considered interval, could be unset.
    /// \param _series Returned series of intake in the considered interval.
    /// \return 1 if the intake lies in the specified interval, 0 otherwise.
    ///
    /// \pre _start IS NOT unset
    /// \pre _end IS unset OR _end > _start
    /// \post if (inside time bounds) { _series[OUT] = _series[IN] + intake } else { _series[OUT] = _series[IN] }
    /// \post FORALL intake IN extracted_intakes, intake.time IN [_start, _end)
    int extract(const WeeklyDose &_dosage, const DateTime &_start, const DateTime &_end, IntakeSeries &_series);
};

}
}

#endif // TUCUXI_CORE_INTAKEEXTRACTOR_H
