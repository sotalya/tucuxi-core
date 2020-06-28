/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/sampleextractor.h"

namespace Tucuxi {
namespace Core {

template<class T>
bool contains(std::vector<T> _vector, T _s) {
    if(std::find(_vector.begin(), _vector.end(), _s) != _vector.end()) {
        return true;
    } else {
        return false;
    }
}


bool sortSamples(const SampleEvent &_a, const SampleEvent &_b)
{
    return _a.getEventTime() < _b.getEventTime();
}

ComputingStatus SampleExtractor::extract(
        const Samples &_samples,
        const AnalyteSet *_analyteGroup,
        const DateTime &_start,
        const DateTime &_end,
        SampleSeries &_series)
{
    int nbRelevantSamples = 0;
    AnalyteId singleAnalyte = AnalyteId("");
    std::vector<AnalyteId> potentialAnalyteIds;
    for (const auto &analyte : _analyteGroup->getAnalytes()) {
        potentialAnalyteIds.push_back(analyte->getAnalyteId());
    }
    for (const auto & sample : _samples) {
        if (contains(potentialAnalyteIds, sample->getAnalyteId())) {
            if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
                _series.push_back(SampleEvent(sample->getDate(), UnitManager::translateToUnit(sample->getValue(), sample->getUnit(), Unit("ug/l"))));
                nbRelevantSamples ++;
                if (singleAnalyte.toString().empty()) {
                    singleAnalyte = sample->getAnalyteId();
                }
                else if (sample->getAnalyteId() != singleAnalyte) {
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
    std::sort(_series.begin(), _series.end(), [](const SampleEvent &_a, const SampleEvent &_b)
    {
        return _a.getEventTime() < _b.getEventTime();
    });

    return ComputingStatus::Ok;
}

ComputingStatus SampleExtractor::extract(
        const Samples &_samples,
        const DateTime &_start,
        const DateTime &_end,
        SampleSeries &_series)
{
    int nbRelevantSamples = 0;
    AnalyteId singleAnalyte = AnalyteId("");
    for (const auto & sample : _samples) {
        if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            _series.push_back(SampleEvent(sample->getDate(), UnitManager::translateToUnit(sample->getValue(), sample->getUnit(), Unit("ug/l"))));
            nbRelevantSamples ++;
            if (singleAnalyte.toString().empty()) {
                singleAnalyte = sample->getAnalyteId();
            }
            else if (sample->getAnalyteId() != singleAnalyte) {
                // We currently do not support multi-analytes for measures
                Tucuxi::Common::LoggerHelper logger;
                logger.error("Multi-analytes measures are not yet supported");
                return ComputingStatus::SampleExtractionError;
            }
        }
    }

    // Sort the samples in chronological order
    //std::sort(_series.begin(), _series.end(), sortSamples);
    std::sort(_series.begin(), _series.end(), [](const SampleEvent &_a, const SampleEvent &_b)
    {
        return _a.getEventTime() < _b.getEventTime();
    });

    return ComputingStatus::Ok;
}

} // namespace Core
} // namespace Tucuxi

