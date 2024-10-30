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


#ifndef TUCUXI_CORE_IRESIDUALERRORMODEL_H
#define TUCUXI_CORE_IRESIDUALERRORMODEL_H


#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The currently implemented error models
///
enum class ResidualErrorType
{
    PROPORTIONAL,
    EXPONENTIAL,
    ADDITIVE,
    MIXED,
    PROPEXP, // Mix of proportional for loglikelihood and exponential for percentiles
    ///! Not yet supported
    SOFTCODED,
    NONE
};

///
/// \brief The IResidualErrorModel class interface
/// This interface is meant to be subclassed by any residual error model.
/// It exposes methods to apply epsilons to a set of concentrations and also to
/// calculate the likelihood of an observed value compared to an expected.
class IResidualErrorModel
{
public:
    /// \brief virtual destructor
    virtual ~IResidualErrorModel() {}

    /// \brief Indicates if there is really an error model in it
    /// \return true if no error model is implemented within the class, false else
    virtual bool isEmpty() const = 0;

    /// \brief Applies the epsilons to a single concentrations
    /// \param _concentration The concentration to be modified.
    /// \param _eps The vector of epsilons
    /// This method supports a vector of epsilons and is responsible to modify the concentration.
    virtual void applyEpsToValue(Concentration& _concentration, const Deviations& _eps) const = 0;

    /// \brief Applies the epsilons to a set of concentrations
    /// \param _concentrations The set of concentrations.
    /// \param _eps The vector of epsilons
    /// This method supports a vector of epsilons and is responsible to modify the concentrations.
    virtual void applyEpsToArray(Concentrations& _concentrations, const Deviations& _eps) const = 0;

    /// \brief Calculates the likelihood of an observed value compared to an expected
    /// \param _expected Expected value
    /// \param _observed Observed value
    /// \return The calculated likelihood
    virtual Value calculateSampleLikelihood(Value _expected, Value _observed) const = 0;

    /// \brief Returns the number of epsilons requested by a specific implementation
    /// \return The number of epsilons requested by applyEpsToArray
    virtual size_t nbEpsilons() const = 0;
};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_IRESIDUALERRORMODEL_H
