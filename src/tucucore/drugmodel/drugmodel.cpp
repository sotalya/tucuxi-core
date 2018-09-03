
/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/covariateevent.h"

namespace Tucuxi {
namespace Core {

bool ParameterDefinitionIterator::isDone() const
{
    size_t nTotal = 0;
    const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteId, m_formulation, m_route);
    if (params1 != nullptr) {
        nTotal += params1->getNbParameters();
    }
    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteId);
    if (params2 != nullptr) {
        nTotal += params2->getNbParameters();
    }
    return (m_index >= nTotal);
}

const ParameterDefinition* ParameterDefinitionIterator::operator*()
{
    size_t index = m_index;
    const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteId, m_formulation, m_route);
    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteId);

    if (params1 != nullptr) {
        if (index < params1->getNbParameters()) {
            return params1->getParameter(index);
        }
        index -= params1->getNbParameters();
    }    
    if (params2 != nullptr && index < params2->getNbParameters()) {
        return params2->getParameter(index);
    }
    return nullptr;
}

DrugModel::DrugModel()
{
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


const FormulationAndRoutes& DrugModel::getFormulationAndRoutes() const
{
    return m_formulationAndRoutes;
}


void DrugModel::addFormulationAndRoute(std::unique_ptr<FormulationAndRoute> _formulationAndRoute, bool _isDefault)
{
    m_formulationAndRoutes.add(std::move(_formulationAndRoute), _isDefault);
}


void DrugModel::addActiveMoiety(std::unique_ptr<ActiveMoiety> _activeMoiety)
{
    m_activeMoieties.push_back(std::move(_activeMoiety));
}

void DrugModel::setDomain(std::unique_ptr<DrugModelDomain> _domain)
{
    m_domain = std::move(_domain);
}


void DrugModel::setAnalyteSet(std::unique_ptr<AnalyteSet> _analyteSet)
{
    m_analyteSets.push_back(std::move(_analyteSet));
}



}
}
