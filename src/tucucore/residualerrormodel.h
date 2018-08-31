/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"
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

    /// \brief Returns the number of epsilons requested by a specific implementation
    /// \return The number of epsilons requested by applyEpsToArray
    virtual int nbEpsilons() const = 0;

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

    SigmaResidualErrorModel() : m_nbEpsilons(1) {}

    void setSigma(Sigma _sigma) { m_sigma = _sigma;}
    void setErrorModel(ResidualErrorType _errorModel) { m_errorModel = _errorModel;}
    bool isEmpty() const override;
    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override;

    Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const override;

    int nbEpsilons() const override { return m_nbEpsilons; }

protected:
    /// The Sigma vector
    Sigma m_sigma;

    /// The residual error model type
    ResidualErrorType m_errorModel;

    /// Number of epsilons requested by applyEpsToArray()
    int m_nbEpsilons;


};

class EmptyResidualErrorModel : public IResidualErrorModel
{
public:
    bool isEmpty() const override { return true;}

    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override {
        UNUSED_PARAMETER(_concentrations);
        UNUSED_PARAMETER(_eps);
    };

    Value calculateSampleLikelihood(const Value _expected, const Value& _observed) const override {
        UNUSED_PARAMETER(_expected);
        UNUSED_PARAMETER(_observed);
        return 0.0;
    };

    int nbEpsilons() const override { return 0; }

};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
// static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;
#define EMPTY_RESIDUAL_ERROR_MODEL EmptyResidualErrorModel()

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
