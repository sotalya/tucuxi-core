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


#ifndef TUCUXI_CORE_SAMPLE_H
#define TUCUXI_CORE_SAMPLE_H

#include <vector>

#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/analyte.h"

using Tucuxi::Common::DateTime; // NOLINT(google-global-names-in-headers)

namespace Tucuxi {
namespace Core {


//class ConcentrationData
//{
//public:
//    // Constructors
//    ConcentrationData() = delete;

//    ConcentrationData(
//        AnalyteId _analyteID,
//        Value _value,
//        Unit _unit
//    );

//    ConcentrationData(ConcentrationData& _other) = delete;

//    ///
//    /// \brief Returns the analyte ID
//    /// \return The analyte ID
//    ///
//    AnalyteId getAnalyteID() const;

//    ///
//    /// \brief returns the measured value
//    /// \return The measured value
//    ///
//    Value getValue() const;

//    ///
//    /// \brief Returns the unit of the measure
//    /// \return The unit of the measure
//    ///
//    Unit getUnit() const;

//protected:

//    /// Analyte ID of the measured sample
//    const AnalyteId m_analyteID;
//    /// Measured value
//    const Value m_value;
//    /// Unit of the measure
//    const Unit m_unit;
//};

///
/// \brief A Sample measured in a patient's blood
/// This class embeds all data of a measure necessary for performing
/// any calculation (a posteriori prediction).
/// It does not embed any administrative data, but shall identify the
/// analyte with an ID understandable by the processing service.
///
class Sample
{
public:
    ///
    /// \brief Sample constructor
    /// \param _date Date of measures
    /// \param _analyteId ID of the measured analyte
    /// \param _value Value of concentration
    /// \param _unit Unit of the value
    ///
    Sample(DateTime _date, AnalyteId _analyteId, Value _value, TucuUnit _unit, Value _weight = 1.0);

    ///
    /// \brief Virtual destructor
    ///
    virtual ~Sample();

    ///
    /// \brief Returns the date of measure
    /// \return The date of measure
    ///
    DateTime getDate() const;

    ///
    /// \brief Returns the sample ID
    /// \return Sample ID
    ///
    //    std::string getSampleId() const;

    ///
    /// \brief Returns the analyte ID
    /// \return The analyte ID
    ///
    AnalyteId getAnalyteID() const;

    ///
    /// \brief returns the measured value
    /// \return The measured value
    ///
    Value getValue() const;

    ///
    /// \brief Returns the unit of the measure
    /// \return The unit of the measure
    ///
    TucuUnit getUnit() const;

    ///
    /// \brief Returns the weight of the sample
    /// \return The sample weight
    ///
    Value getWeight() const;


protected:
    /// Sample ID
    //    const std::string m_sampleID;

    /// Date of measure (date + time)
    DateTime m_date;

    /// Analyte ID of the measured sample
    const AnalyteId m_analyteID;

    /// Measured value
    const Value m_value;

    /// Unit of the measure
    const TucuUnit m_unit;

    /// Sample weight
    const Value m_weight;
};

/// A vector of unique_ptr on Sample objects
typedef std::vector<std::unique_ptr<Sample> > Samples;



} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLE_H
