/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_IRESIDUALERRORMODEL_H
#define TUCUXI_CORE_IRESIDUALERRORMODEL_H


#include "tucucore/definitions.h"
#include "tucucommon/general.h"
#include "tucucore/drugdefinitions.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The currently implemented error models
///
enum class ResidualErrorType {
    PROPORTIONAL,
    EXPONENTIAL,
    ADDITIVE,
    MIXED,
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
    virtual void applyEpsToValue(Concentration &_concentration, const Deviations &_eps) const = 0;

    /// \brief Applies the epsilons to a set of concentrations
    /// \param _concentrations The set of concentrations.
    /// \param _eps The vector of epsilons
    /// This method supports a vector of epsilons and is responsible to modify the concentrations.
    virtual void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const = 0;

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
