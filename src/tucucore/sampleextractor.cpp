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
            if ((sample->getDate() >= _start) && (sample->getDate() <= _end)) {
                //if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
                TUCU_TRY
                {
                    _series.push_back(SampleEvent(
                            sample->getDate(),
                            UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                                    sample->getValue(), sample->getUnit(), _toUnit),
                            sample->getWeight()));
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
        if ((sample->getDate() >= _start) && (sample->getDate() <= _end)) {
            //    if ((sample->getDate() > _start) && (sample->getDate() < _end)) {
            TUCU_TRY
            {
                _series.push_back(SampleEvent(
                        sample->getDate(),
                        UnitManager::convertToUnit<UnitManager::UnitType::Concentration>(
                                sample->getValue(), sample->getUnit(), _toUnit),
                        sample->getWeight()));
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
