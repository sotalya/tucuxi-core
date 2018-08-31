/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_SAMPLE_H
#define TUCUXI_CORE_SAMPLE_H

#include <vector>

#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"

using Tucuxi::Common::DateTime;

namespace Tucuxi {
namespace Core {

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
    Sample(DateTime _date, std::string _analyteId, Value _value, Unit _unit);

    ///
    /// \brief Returns the analyte ID
    /// \return The analyte ID
    ///
    std::string getAnalyteId() const;

    ///
    /// \brief Returns the date of measure
    /// \return The date of measure
    ///
    DateTime getDate() const;

    ///
    /// \brief returns the measured value
    /// \return The measured value
    ///
    Value getValue() const;

    ///
    /// \brief Returns the unit of the measure
    /// \return The unit of the measure
    ///
    Unit getUnit() const;

protected:

    /// Analyte ID of the measured sample
    std::string m_analyteId;

    /// Date of measure (date + time)
    DateTime m_date;

    /// Measured value
    Value m_value;

    /// Unit of the measure
    Unit m_unit;
};

/// A vector of unique_ptr on Sample objects
typedef std::vector<std::unique_ptr<Sample> > Samples;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_SAMPLE_H
