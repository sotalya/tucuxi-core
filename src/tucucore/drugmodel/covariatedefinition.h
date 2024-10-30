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


#ifndef COVARIATEDEFINITION_H
#define COVARIATEDEFINITION_H

#include <utility>
#include <vector>

#include "tucucommon/translatablestring.h"
#include "tucucommon/unit.h"
#include "tucucommon/utils.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/invariants.h"
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
    CovariateDefinition(
            const std::string& _id,
            const std::string& _value,
            std::unique_ptr<Operation> _operation,
            const CovariateType _type = CovariateType::Standard,
            const DataType _dataType = DataType::Double,
            Tucuxi::Common::TranslatableString _name = Tucuxi::Common::TranslatableString())
        : PopulationValue(_id, Tucuxi::Common::Utils::stringToValue(_value, _dataType), std::move(_operation)),
          m_type{_type}, m_dataType{_dataType}, m_interpolationType{InterpolationType::Direct}, m_name(std::move(_name))
    {
        if (_type != CovariateType::Standard) {
            // For the age, reset all the values set by the user to appropriate ones.
            m_refreshPeriod.clear();
            m_operation = nullptr;
        }
    }

    /// \brief Get the covariate's type.
    /// \return Covariate's type.
    CovariateType getType() const
    {
        return m_type;
    }

    /// \brief Get the data type.
    /// \return Data type.
    DataType getDataType() const
    {
        return m_dataType;
    }

    /// \brief Set the interpolation type.
    /// \param _interpolationType Selected interpolation type.
    void setInterpolationType(const InterpolationType& _interpolationType)
    {
        m_interpolationType = _interpolationType;
    }

    /// \brief Get the interpolation type.
    /// \return Selected interpolation type.
    InterpolationType getInterpolationType() const
    {
        return m_interpolationType;
    }

    /// \brief Get the data's unit of measure.
    /// \return Data's unit of measure.
    TucuUnit getUnit() const
    {
        return m_unit;
    }

    /// \brief Set the data's unit of measure.
    /// \param _unit Data's unit of measure.
    void setUnit(const TucuUnit& _unit)
    {
        m_unit = _unit;
    }

    /// \brief Get the refresh period.
    /// \return Refresh period.
    Tucuxi::Common::Duration getRefreshPeriod() const
    {
        return m_refreshPeriod;
    }

    /// \brief Set the refresh period.
    /// \param _refreshPeriod Refresh period to set.
    void setRefreshPeriod(const Tucuxi::Common::Duration& _refreshPeriod)
    {
        m_refreshPeriod = _refreshPeriod;
    }

    /// \brief Set the validation operation
    /// \param _operation Operation used to validate the covariate value
    void setValidation(std::unique_ptr<Operation> _validation)
    {
        m_validation = std::move(_validation);
    }

    Operation* getValidation() const
    {
        return m_validation.get();
    }

    ///
    /// \brief Set the name of the covariate
    /// \param _name TranslatableString representing the covariate
    ///
    void setName(const Tucuxi::Common::TranslatableString& _name)
    {
        m_name = _name;
    }

    ///
    /// \brief Get the name of the covariate
    /// \return A TranslatableString corresponding to the name of the covariate
    ///
    const Tucuxi::Common::TranslatableString& getName() const
    {
        return m_name;
    }

    ///
    /// \brief Set the description of the covariate
    /// \param _description TranslatableString describing the covariate
    ///
    void setDescription(const Tucuxi::Common::TranslatableString& _description)
    {
        m_description = _description;
    }

    ///
    /// \brief Get the description of the covariate
    /// \return A TranslatableString describing the covariate
    ///
    const Tucuxi::Common::TranslatableString& getDescription() const
    {
        return m_description;
    }

    ///
    /// \brief Set the validation error message
    /// \param _validationErrorMessage TranslatableString corresponding to the error message
    ///
    void setValidationErrorMessage(const Tucuxi::Common::TranslatableString& _validationErrorMessage)
    {
        m_validationErrorMessage = _validationErrorMessage;
    }

    ///
    /// \brief Get the validation error message
    /// \return A TranslatableString representing the validation error message
    ///
    const Tucuxi::Common::TranslatableString& getValidationErrorMessage() const
    {
        return m_validationErrorMessage;
    }


    INVARIANTS(INVARIANT(Invariants::INV_COVARIATEDEFINITION_0001, (!m_id.empty()), "A covariate has no Id"))
protected:
    /// \brief Covariate type.
    CovariateType m_type;
    /// \brief Data type.
    DataType m_dataType;
    /// \brief Interpolation type.
    InterpolationType m_interpolationType;
    /// \brief Unit of measure used for the value.
    Tucuxi::Common::TucuUnit m_unit;
    /// \brief Interval between two interpolated values.
    Tucuxi::Common::Duration m_refreshPeriod;
    /// \brief Operation to validate the value of the covariate
    std::unique_ptr<Operation> m_validation;
    /// \brief Name of the covariate
    Tucuxi::Common::TranslatableString m_name;
    /// \brief Description of the covariate
    Tucuxi::Common::TranslatableString m_description;
    /// \brief Error message for the validation of the covariate
    Tucuxi::Common::TranslatableString m_validationErrorMessage;
};

/// \brief List of covariate definitions.
typedef std::vector<std::unique_ptr<CovariateDefinition>> CovariateDefinitions;

/// \brief Iterator in the list of covariate definitions.
typedef std::vector<std::unique_ptr<CovariateDefinition>>::const_iterator cdIterator_t;

} // namespace Core
} // namespace Tucuxi

#endif // COVARIATEDEFINITION_H
