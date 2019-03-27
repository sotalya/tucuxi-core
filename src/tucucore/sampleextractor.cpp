/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/sampleextractor.h"

namespace Tucuxi {
namespace Core {

SampleExtractor::Result SampleExtractor::extract(const Samples &_samples, const DateTime &_start, const DateTime &_end, SampleSeries &_series)
{
    int nbRelevantSamples = 0;
    AnalyteId singleAnalyte = AnalyteId("");
    for (const auto & sample : _samples) {
        if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            _series.push_back(SampleEvent(sample->getDate(), translateToUnit(sample->getValue(), sample->getUnit(), Unit("ug/l"))));
            nbRelevantSamples ++;
            if (singleAnalyte.toString().size() == 0) {
                singleAnalyte = sample->getAnalyteId();
            }
            else if (sample->getAnalyteId() != singleAnalyte) {
                // We currently do not support multi-analytes for measures
                Tucuxi::Common::LoggerHelper logger;
                logger.error("Multi-analytes measures are not yet supported");
                return Result::ExtractionError;
            }
        }
    }
    return Result::Ok;
}


} // namespace Core
} // namespace Tucuxi

