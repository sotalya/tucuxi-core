/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"
#include "tucucommon/general.h"
#include "tucucore/drugdefinitions.h"

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
        SOFTCODED,
        NONE
    };

    SigmaResidualErrorModel() : m_nbEpsilons(1) {}

    void setFormula(std::unique_ptr<Operation> _formula) {m_formula = std::move(_formula);}
    void addOriginalSigma(std::unique_ptr<PopulationValue> _sigma) {m_originalSigmas.push_back(std::move(_sigma));}
    void setSigma(Sigma _sigma) { m_sigma = _sigma;}
    void setErrorModel(ResidualErrorType _errorModel) { m_errorModel = _errorModel;}
    bool isEmpty() const override;
    void applyEpsToValue(Concentration &_concentration, const Deviations &_eps) const override;
    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override;

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override;

    int nbEpsilons() const override { return m_nbEpsilons; }

    void generatePopulationSigma(const Unit &_fromUnit, const Unit &_toUnit) {
        m_sigma = Sigma(m_originalSigmas.size());
        for(std::size_t i = 0;i < m_originalSigmas.size(); i++) {
            if (m_errorModel == ResidualErrorType::ADDITIVE) {
                m_sigma[i] = translateToUnit(m_originalSigmas.at(i)->getValue(), _fromUnit, _toUnit);
            }
            else {
                m_sigma[i] = m_originalSigmas.at(i)->getValue();
            }
        }
    }

protected:

    std::unique_ptr<Operation> m_formula;

    std::vector<std::unique_ptr<PopulationValue> > m_originalSigmas;

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


    void applyEpsToValue(Concentration &_concentration, const Deviations &_eps) const override {
        UNUSED_PARAMETER(_concentration);
        UNUSED_PARAMETER(_eps);
    }

    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override {
        UNUSED_PARAMETER(_concentrations);
        UNUSED_PARAMETER(_eps);
    }

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override {
        UNUSED_PARAMETER(_expected);
        UNUSED_PARAMETER(_observed);
        return 0.0;
    }

    int nbEpsilons() const override { return 0; }

};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
// static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;
#define EMPTY_RESIDUAL_ERROR_MODEL EmptyResidualErrorModel()

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
