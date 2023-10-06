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


#ifndef TUCUXI_CORE_SAMPLEEXTRACTOR_H
#define TUCUXI_CORE_SAMPLEEXTRACTOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/drugtreatment/sample.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {

class SampleExtractor
{
public:
    ///
    /// \brief extract relevant samples
    /// \param _samples The list of available samples
    /// \param _analyteGroup The analyte group in which the sample analyte has to be
    /// \param _start The start time of the range of interest
    /// \param _end The end time of the range of interest
    /// \param _toUnit The unit into which the sample value shall be converted
    /// \param _series The output series containing the relevant samples
    /// \return Result::Ok if everything went well
    ///
    /// This function also converts the samples to ug/l.
    ///
    ComputingStatus extract(
            const Samples& _samples,
            const AnalyteSet* _analyteGroup,
            const DateTime& _start,
            const DateTime& _end,
            const TucuUnit& _toUnit,
            SampleSeries& _series);


    ///
    /// \brief extract all relevant samples
    /// \param _samples The list of available samples
    /// \param _start The start time of the range of interest
    /// \param _end The end time of the range of interest
    /// \param _toUnit The unit into which the sample value shall be converted
    /// \param _series The output series containing the relevant samples
    /// \return Result::Ok if everything went well
    ///
    /// This function also converts the samples to ug/l.
    ///
    ComputingStatus extract(
            const Samples& _samples,
            const DateTime& _start,
            const DateTime& _end,
            const TucuUnit& _toUnit,
            SampleSeries& _series);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLEEXTRACTOR_H
