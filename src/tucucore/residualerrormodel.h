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


#ifndef TUCUXI_ROOT_RESIDUALERRORMODEL_H
#define TUCUXI_ROOT_RESIDUALERRORMODEL_H

#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/iresidualerrormodel.h"

namespace Tucuxi {
namespace Core {


class SigmaResidualErrorModel : public IResidualErrorModel
{
public:
    SigmaResidualErrorModel() : m_errorModel(ResidualErrorType::NONE), m_nbEpsilons(1) {}

    void setSigma(Sigma _sigma);
    void setErrorModel(ResidualErrorType _errorModel);
    bool isEmpty() const override;
    void applyEpsToValue(Concentration& _concentration, const Deviations& _eps) const override;
    void applyEpsToArray(Concentrations& _concentrations, const Deviations& _eps) const override;

    ///
    /// \brief calculateSampleLikelihood
    /// \param _expected
    /// \param _observed
    /// \return
    ///
    /// \unittest{TestResidualErrorModel::testLogLikelihood()}
    ///
    Value calculateSampleLikelihood(Value _expected, Value _observed) const override;

    size_t nbEpsilons() const override
    {
        return m_nbEpsilons;
    }

protected:
    /// The Sigma vector
    Sigma m_sigma;

    /// The residual error model type
    ResidualErrorType m_errorModel;

    /// Number of epsilons requested by applyEpsToArray()
    size_t m_nbEpsilons;
};


class SoftCodedResidualErrorModel : public IResidualErrorModel
{
public:
    SoftCodedResidualErrorModel() : m_nbEpsilons(1) {}

    void setApplyFormula(std::unique_ptr<Operation> _applyFormula)
    {
        m_applyFormula = std::move(_applyFormula);
    }
    void setLikelyhoodFormula(std::unique_ptr<Operation> _likelyhoodFormula)
    {
        m_likelyhoodFormula = std::move(_likelyhoodFormula);
    }
    void setSigma(Sigma _sigma)
    {
        m_sigma = std::move(_sigma);
    }
    bool isEmpty() const override;
    void applyEpsToValue(Concentration& _concentration, const Deviations& _eps) const override;
    void applyEpsToArray(Concentrations& _concentrations, const Deviations& _eps) const override;

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override;

    size_t nbEpsilons() const override
    {
        return m_nbEpsilons;
    }

protected:
    std::unique_ptr<Operation> m_applyFormula;
    std::unique_ptr<Operation> m_likelyhoodFormula;

    /// The Sigma vector
    Sigma m_sigma;

    /// Number of epsilons requested by applyEpsToArray()
    size_t m_nbEpsilons;
};



class EmptyResidualErrorModel : public IResidualErrorModel
{
public:
    bool isEmpty() const override
    {
        return true;
    }


    void applyEpsToValue(Concentration& _concentration, const Deviations& _eps) const override
    {
        FINAL_UNUSED_PARAMETER(_concentration);
        FINAL_UNUSED_PARAMETER(_eps);
    }

    void applyEpsToArray(Concentrations& _concentrations, const Deviations& _eps) const override
    {
        FINAL_UNUSED_PARAMETER(_concentrations);
        FINAL_UNUSED_PARAMETER(_eps);
    }

    Value calculateSampleLikelihood(Value _expected, Value _observed) const override
    {
        FINAL_UNUSED_PARAMETER(_expected);
        FINAL_UNUSED_PARAMETER(_observed);
        return 0.0;
    }

    size_t nbEpsilons() const override
    {
        return 0;
    }
};

//#define EMPTY_RESIDUAL_ERROR_MODEL ResidualErrorModel{EMPTY_SIGMA, ResidualErrorType::NONE}
// static const ResidualErrorModel EMPTY_RESIDUAL_ERROR_MODEL;
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EMPTY_RESIDUAL_ERROR_MODEL EmptyResidualErrorModel()

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_ROOT_RESIDUALERRORMODEL_H
