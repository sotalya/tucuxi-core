
/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/covariateevent.h"

namespace Tucuxi {
namespace Core {

void ParameterDefinitionIterator::build()
{
    m_total = 0;

    // Depending on the constructor invoked, we build the list of absorption parameters
    if (m_fullFormulationAndRoutes.size() > 0) {
        for (const auto & f : m_fullFormulationAndRoutes) {
            const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, f->getFormulationAndRoute());
            if (params1 != nullptr) {
                params1a.push_back(params1);
                m_total += params1->getNbParameters();
            }
        }
    }
    else {
        const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, m_formulation, m_route);
        if (params1 != nullptr) {
            params1a.push_back(params1);
            m_total += params1->getNbParameters();
        }
    }

    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteGroupId);
    if (params2 != nullptr) {
        m_total += params2->getNbParameters();
    }


    // Add the absorption parameters
    for(const auto & params : params1a) {
        for (size_t i = 0;i < params->getNbParameters(); i++) {
            vector.push_back({params->getParameter(i)->getId(), params->getParameter(i)->isVariable()});
        }
    }

    // Add the elimination parameters
    for (size_t i = 0;i < params2->getNbParameters(); i++) {
        vector.push_back({params2->getParameter(i)->getId(), params2->getParameter(i)->isVariable()});
    }

    // We sort alphabetically the parameter Ids
    std::sort(vector.begin(), vector.end(), [&] (const ddd v1, const ddd v2) {
        if (v1.isVariable && !v2.isVariable)
            return true;
        if (!v1.isVariable && v2.isVariable)
            return false;
        return v1.id < v2.id;
    });

}

bool ParameterDefinitionIterator::isDone() const
{
    return (m_index >= m_total);
}


const ParameterDefinition* ParameterDefinitionIterator::operator*()
{

    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteGroupId);

    std::string curId = vector[m_index].id;

    for(const auto & params : params1a) {
        for (size_t i = 0;i < params->getNbParameters(); i++) {
            if (params->getParameter(i)->getId() == curId)
                return params->getParameter(i);
        }
    }

    for (size_t i = 0;i < params2->getNbParameters(); i++) {
        if (params2->getParameter(i)->getId() == curId)
            return params2->getParameter(i);
    }

    assert (false);

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



}
}
