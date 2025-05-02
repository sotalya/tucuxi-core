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


#include <utility>

#include "tucucore/drugmodel/drugmodel.h"

#include "tucucore/covariateevent.h"

namespace Tucuxi {
namespace Core {

void ParameterDefinitionIterator::build()
{
    m_total = 0;

    // Depending on the constructor invoked, we build the list of absorption parameters
    if (!m_fullFormulationAndRoutes.empty()) {
        for (const auto& f : m_fullFormulationAndRoutes) {
            const ParameterSetDefinition* parameter =
                    m_model.getAbsorptionParameters(m_analyteGroupId, f->getFormulationAndRoute());
            if (parameter != nullptr) {
                m_absorptionParameters.push_back(parameter);
                m_total += parameter->getNbParameters();
            }
        }
    }
    else if (!m_formulationAndRoutes.empty()) {
        for (const auto& f : m_formulationAndRoutes) {
            const ParameterSetDefinition* parameter = m_model.getAbsorptionParameters(m_analyteGroupId, f);
            if (parameter != nullptr) {
                m_absorptionParameters.push_back(parameter);
                m_total += parameter->getNbParameters();
            }
        }
    }
    else if (!m_treatmentFormulationAndRoutes.empty()) {
        for (const auto& f : m_treatmentFormulationAndRoutes) {
            const ParameterSetDefinition* parameter = m_model.getAbsorptionParameters(m_analyteGroupId, f);
            if (parameter != nullptr) {
                m_absorptionParameters.push_back(parameter);
                m_total += parameter->getNbParameters();
            }
        }
    }
    else {
        const ParameterSetDefinition* parameter =
                m_model.getAbsorptionParameters(m_analyteGroupId, m_formulation, m_route);
        if (parameter != nullptr) {
            m_absorptionParameters.push_back(parameter);
            m_total += parameter->getNbParameters();
        }
    }

    const ParameterSetDefinition* dispositionParameters = m_model.getDispositionParameters(m_analyteGroupId);
    if (dispositionParameters != nullptr) {
        m_total += dispositionParameters->getNbParameters();
    }


    // Add the absorption parameters
    for (const auto& params : m_absorptionParameters) {
        for (size_t i = 0; i < params->getNbParameters(); i++) {
            m_parametersVector.push_back({params->getParameter(i)->getId(), params->getParameter(i)->isVariable()});
        }
    }

    // Add the elimination parameters
    for (size_t i = 0; i < dispositionParameters->getNbParameters(); i++) {
        m_parametersVector.push_back(
                {dispositionParameters->getParameter(i)->getId(),
                 dispositionParameters->getParameter(i)->isVariable()});
    }

    // We sort alphabetically the parameter Ids
    std::sort(
            m_parametersVector.begin(),
            m_parametersVector.end(),
            [&](const ParameterInfo& _v1, const ParameterInfo& _v2) {
                if (_v1.isVariable && !_v2.isVariable) {
                    return true;
                }
                if (!_v1.isVariable && _v2.isVariable) {
                    return false;
                }
                return _v1.id < _v2.id;
            });
}

bool ParameterDefinitionIterator::isDone() const
{
    return (m_index >= m_total);
}


const ParameterDefinition* ParameterDefinitionIterator::operator*()
{
    // We get the parameter Id of the current parameter we are looking for
    std::string curId = m_parametersVector[m_index].id;

    // Iterate over the absorption parameters to find the parameter Id
    for (const auto& parameterSet : m_absorptionParameters) {
        for (size_t i = 0; i < parameterSet->getNbParameters(); i++) {
            if (parameterSet->getParameter(i)->getId() == curId) {
                return parameterSet->getParameter(i);
            }
        }
    }

    // Iterate over the disposition parameters to find the parameter Id
    const ParameterSetDefinition* dispositionParameters = m_model.getDispositionParameters(m_analyteGroupId);

    for (size_t i = 0; i < dispositionParameters->getNbParameters(); i++) {
        if (dispositionParameters->getParameter(i)->getId() == curId) {
            return dispositionParameters->getParameter(i);
        }
    }

    // This really should never happen
    throw std::runtime_error("Internal error: A paramter from a parameter set could not be extracted.");

    return nullptr;
}

DrugModel::DrugModel()
{
    m_formulationAndRoutes = std::make_unique<FormulationAndRoutes>();
}


void DrugModel::setTimeToSteadyState(Tucuxi::Common::Duration _time)
{
    m_timeToSteadyState = _time;
}


Tucuxi::Common::Duration DrugModel::getTimeToSteadyState() const
{
    return m_timeToSteadyState;
}


void DrugModel::addCovariate(std::unique_ptr<CovariateDefinition> _covariate)
{
    m_covariates.push_back(std::move(_covariate));
}


const CovariateDefinitions& DrugModel::getCovariates() const
{
    return m_covariates;
}

bool DrugModel::hasDoseCovariate() const
{
    for (const auto& covariate : m_covariates) {
        if (covariate->getType() == CovariateType::Dose) {
            return true;
        }
    }
    return false;
}


CovariateDefinition* DrugModel::getDoseCovariate() const
{
    for (const auto& covariate : m_covariates) {
        if (covariate->getType() == CovariateType::Dose) {
            return covariate.get();
        }
    }
    return nullptr;
}

const FormulationAndRoutes& DrugModel::getFormulationAndRoutes() const
{
    return *m_formulationAndRoutes;
}


void DrugModel::addFormulationAndRoute(std::unique_ptr<FullFormulationAndRoute> _formulationAndRoute, bool _isDefault)
{
    m_formulationAndRoutes->add(std::move(_formulationAndRoute), _isDefault);
}


void DrugModel::addActiveMoiety(std::unique_ptr<ActiveMoiety> _activeMoiety)
{
    m_activeMoieties.push_back(std::move(_activeMoiety));
}

void DrugModel::setDomain(std::unique_ptr<DrugModelDomain> _domain)
{
    m_domain = std::move(_domain);
}

void DrugModel::setMetadata(std::unique_ptr<DrugModelMetadata> _metadata)
{
    m_metadata = std::move(_metadata);
}

void DrugModel::addAnalyteSet(std::unique_ptr<AnalyteSet> _analyteSet)
{
    m_analyteSets.push_back(std::move(_analyteSet));
}



} // namespace Core
} // namespace Tucuxi
