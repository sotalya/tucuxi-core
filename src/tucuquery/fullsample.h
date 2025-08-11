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


#ifndef FULLSAMPLE_H
#define FULLSAMPLE_H

#include <string>

#include "tucucore/drugtreatment/sample.h"

namespace Tucuxi {
namespace Query {

/// \brief Subclass of Sample that is more administration-oriented because
///        it includes the sample identifier.
class FullSample : public Core::Sample
{
public:
    ///
    /// \brief Sample constructor
    /// \param _id Unique identifier
    /// \param _date Date of measures
    /// \param _analyteId ID of the measured analyte
    /// \param _value Value of concentration
    /// \param _unit Unit of the value
    /// \param _weight The sample weight
    ///
    FullSample(
            std::string _id,
            Common::DateTime _date,
            Core::AnalyteId _analyteId,
            Core::Value _value,
            TucuUnit _unit,
            Core::Value _weight = 1.0);

    /// \brief Returns the sample ID
    /// \return Sample ID
    std::string getSampleId() const;

protected:
    /// Sample ID
    const std::string m_sampleID;
};

/// A vector of unique_ptr on FullSample objects
typedef std::vector<std::unique_ptr<FullSample> > FullSamples;

} // namespace Query
} // namespace Tucuxi

#endif // FULLSAMPLE_H
