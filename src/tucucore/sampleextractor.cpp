//@@lisence@@

#include "tucucore/sampleextractor.h"

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

using Tucuxi::Common::UnitManager;

namespace Tucuxi {
namespace Core {

template<class T>
bool contains(std::vector<T> _vector, T _s)
{
    return (std::find(_vector.begin(), _vector.end(), _s) != _vector.end());
}


bool sortSamples(const SampleEvent& _a, const SampleEvent& _b)
{
    return _a.getEventTime() < _b.getEventTime();
}

ComputingStatus SampleExtractor::extract(
        const Samples& _samples,
        const AnalyteSet* _analyteGroup,
        const DateTime& _start,
        const DateTime& _end,
        const TucuUnit& _toUnit,
        SampleSeries& _series)
{
    int nbRelevantSamples = 0;
    AnalyteId singleAnalyte = AnalyteId("");
    std::vector<AnalyteId> potentialAnalyteIds;
    for (const auto& analyte : _analyteGroup->getAnalytes()) {
        potentialAnalyteIds.push_back(analyte->getAnalyteId());
    }
    for (const auto& sample : _samples) {
        if (contains(potentialAnalyteIds, sample->getAnalyteID())) {
            if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
                TUCU_TRY
                {
                    _series.push_back(SampleEvent(
                            sample->getDate(),
                            UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                                    sample->getValue(), sample->getUnit(), _toUnit)));
                }
                TUCU_CATCH(std::invalid_argument & e)
                {
                    Tucuxi::Common::LoggerHelper logger;
                    logger.error("Sample unit not handled");
                }
                nbRelevantSamples++;
                if (singleAnalyte.toString().empty()) {
                    singleAnalyte = sample->getAnalyteID();
                }
                else if (sample->getAnalyteID() != singleAnalyte) {
                    // We currently do not support multi-analytes for measures
                    Tucuxi::Common::LoggerHelper logger;
                    logger.error("Multi-analytes measures are not yet supported");
                    return ComputingStatus::SampleExtractionError;
                }
            }
        }
    }

    // Sort the samples in chronological order
    //std::sort(_series.begin(), _series.end(), sortSamples);
    std::sort(_series.begin(), _series.end(), [](const SampleEvent& _a, const SampleEvent& _b) {
        return _a.getEventTime() < _b.getEventTime();
    });

    return ComputingStatus::Ok;
}

ComputingStatus SampleExtractor::extract(
        const Samples& _samples,
        const DateTime& _start,
        const DateTime& _end,
        const TucuUnit& _toUnit,
        SampleSeries& _series)
{
    int nbRelevantSamples = 0;
    AnalyteId singleAnalyte = AnalyteId("");
    for (const auto& sample : _samples) {
        if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            TUCU_TRY
            {
                _series.push_back(SampleEvent(
                        sample->getDate(),
                        UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                                sample->getValue(), sample->getUnit(), _toUnit)));
            }
            TUCU_CATCH(std::invalid_argument & e)
            {
                Tucuxi::Common::LoggerHelper logger;
                logger.error("Sample unit not handled");
                return ComputingStatus::SampleExtractionError;
            }
            nbRelevantSamples++;
            if (singleAnalyte.toString().empty()) {
                singleAnalyte = sample->getAnalyteID();
            }
            else if (sample->getAnalyteID() != singleAnalyte) {
                // We currently do not support multi-analytes for measures
                Tucuxi::Common::LoggerHelper logger;
                logger.error("Multi-analytes measures are not yet supported");
                return ComputingStatus::SampleExtractionError;
            }
        }
    }

    // Sort the samples in chronological order
    //std::sort(_series.begin(), _series.end(), sortSamples);
    std::sort(_series.begin(), _series.end(), [](const SampleEvent& _a, const SampleEvent& _b) {
        return _a.getEventTime() < _b.getEventTime();
    });

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi
