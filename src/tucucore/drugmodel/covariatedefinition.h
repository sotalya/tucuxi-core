/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef COVARIATEDEFINITION_H
#define COVARIATEDEFINITION_H

#include <iostream>
#include <vector>

#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/operation.h"

namespace Tucuxi {
namespace Core {

/// \brief Definition of a covariate for a given drug, using the information extracted from the drug's XML file.
class CovariateDefinition : public PopulationValue
{
public:
    /// \brief Create a covariate from its name and value (or operation).
    /// \param _id Name of the covariate.
    /// \param _value Default value given to the covariate, given as a string.
    /// \param _operation Operation used to compute the value of the covariate.
    /// \param _type Type of the covariate.
    /// \param _dataType Type of data contained in the variable
    CovariateDefinition(const std::string &_id, const std::string &_value, Operation *_operation,
                        const CovariateType _type = CovariateType::Standard, const DataType _dataType = DataType::Double) :
        PopulationValue(_id, Tucuxi::Common::Utils::stringToValue(_value, _dataType), _operation),
        m_type{_type}, m_dataType{_dataType}, m_interpolationType{InterpolationType::Direct}
    {
        if (_type != CovariateType::Standard) {
            // For the age, reset all the values set by the user to appropriate ones.
            m_refreshPeriod.clear();
            m_operation = nullptr;
        }
    }

    /// \brief Get the covariate's type.
    /// \return Covariate's type.
    CovariateType getType() const { return m_type; }

    /// \brief Get the data type.
    /// \return Data type.
    DataType getDataType() const { return m_dataType; }

    /// \brief Set the interpolation type.
    /// \param _interpolationType Selected interpolation type.
    void setInterpolationType(const InterpolationType &_interpolationType) { m_interpolationType = _interpolationType; }

    /// \brief Get the interpolation type.
    /// \return Selected interpolation type.
    InterpolationType getInterpolationType() const { return m_interpolationType; }

    /// \brief Get the data's unit of measure.
    /// \return Data's unit of measure.
    Unit getUnit() const { return m_unit; }

    /// \brief Set the data's unit of measure.
    /// \param _unit Data's unit of measure.
    void setUnit(const Unit &_unit) { m_unit = _unit; }

    /// \brief Get the refresh period.
    /// \return Refresh period.
    Tucuxi::Common::Duration getRefreshPeriod() const { return m_refreshPeriod; }

    /// \brief Set the refresh period.
    /// \param _refreshPeriod Refresh period to set.
    void setRefreshPeriod(const Tucuxi::Common::Duration &_refreshPeriod) { m_refreshPeriod = _refreshPeriod; }

protected:
    /// \brief Covariate type.
    CovariateType m_type;
    /// \brief Data type.
    DataType m_dataType;
    /// \brief Interpolation type.
    InterpolationType m_interpolationType;
    /// \brief Unit of measure used for the value.
    Unit m_unit;
    /// \brief Interval between two interpolated values.
    Tucuxi::Common::Duration m_refreshPeriod;
};

/// \brief List of covariate definitions.
typedef std::vector<std::unique_ptr<CovariateDefinition>> CovariateDefinitions;

/// \brief Iterator in the list of covariate definitions.
typedef std::vector<std::unique_ptr<CovariateDefinition>>::const_iterator cdIterator_t;

} // namespace Core
} // namespace Tucuxi

#endif // COVARIATEDEFINITION_H
