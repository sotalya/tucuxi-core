/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"
#include "tucucore/sample.h"
#include "tucucommon/general.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The IResidualErrorModel class interface
/// This interface is meant to be subclassed by any residual error model.
/// It exposes methods to apply epsilons to a set of concentrations and also to
/// calculate the likelihood of an observed value compared to an expected.
class IResidualErrorModel
{
public:

    /// \brief Indicates if there is really an error model in it
    /// \return true if no error model is implemented within the class, false else
    virtual bool isEmpty() const = 0;

    /// \brief Applies the epsilons to a set of concentrations
    /// \param _concentrations The set of concentrations.
    /// \param _eps The vector of epsilons
    /// This method supports a vector of epsilons and is responsible to modify the concentrations.
    virtual void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const = 0;

    /// \brief Calculates the likelihood of an observed value compared to an expected
    /// \param _expected Expected value
    /// \param _observed Observed value
    /// \return The calculated likelihood
    virtual Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const = 0;

};

class SigmaResidualErrorModel : public IResidualErrorModel
{
public:

    // The currently implemented error models
    enum class ResidualErrorType {
        PROPORTIONAL,
        EXPONENTIAL,
        ADDITIVE,
        MIXED,
        NONE
    };

    virtual bool isEmpty() const;
    virtual void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const;

    virtual Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const;

protected:
    /// The Sigma vector
    Sigma sigma;

    /// The residual error model type
    ResidualErrorType errorModel;


};

class EmptyResidualErrorModel : public IResidualErrorModel
{
public:
    virtual bool isEmpty() const { return true;}

    virtual void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const {
        UNUSED_PARAMETER(_concentrations);
        UNUSED_PARAMETER(_eps);
    };

    virtual Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const {
        UNUSED_PARAMETER(_expected);
        UNUSED_PARAMETER(_observed);
        return 0.0;
    };

};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
// static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;
#define EMPTY_RESIDUAL_ERROR_MODEL EmptyResidualErrorModel()

} 
} 

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
