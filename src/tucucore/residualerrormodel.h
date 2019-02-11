/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucore/definitions.h"
#include "tucucommon/general.h"
#include "tucucore/drugdefinitions.h"
#include "tucucore/iresidualerrormodel.h"

namespace Tucuxi {
namespace Core {


class SigmaResidualErrorModel : public IResidualErrorModel
{
public:


    SigmaResidualErrorModel() : m_nbEpsilons(1) {}

    void setSigma(Sigma _sigma) { m_sigma = _sigma;}
    void setErrorModel(ResidualErrorType _errorModel) { m_errorModel = _errorModel;}
    bool isEmpty() const override;
    void applyEpsToValue(Concentration &_concentration, const Deviations &_eps) const override;
    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override;

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override;

    int nbEpsilons() const override { return m_nbEpsilons; }

protected:

    /// The Sigma vector
    Sigma m_sigma;

    /// The residual error model type
    ResidualErrorType m_errorModel;

    /// Number of epsilons requested by applyEpsToArray()
    int m_nbEpsilons;

};


class SoftCodedResidualErrorModel : public IResidualErrorModel
{
public:


    SoftCodedResidualErrorModel() : m_nbEpsilons(1) {}

    void setApplyFormula(std::unique_ptr<Operation> _applyFormula) {m_applyFormula = std::move(_applyFormula);}
    void setLikelyhoodFormula(std::unique_ptr<Operation> _likelyhoodFormula) {m_likelyhoodFormula = std::move(_likelyhoodFormula);}
    void setSigma(Sigma _sigma) { m_sigma = _sigma;}
    bool isEmpty() const override;
    void applyEpsToValue(Concentration &_concentration, const Deviations &_eps) const override;
    void applyEpsToArray(Concentrations &_concentrations, const Deviations &_eps) const override;

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override;

    int nbEpsilons() const override { return m_nbEpsilons; }

protected:

    std::unique_ptr<Operation> m_applyFormula;
    std::unique_ptr<Operation> m_likelyhoodFormula;

    /// The Sigma vector
    Sigma m_sigma;

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
