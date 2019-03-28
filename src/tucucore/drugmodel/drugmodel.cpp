
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

    std::vector<const ParameterSetDefinition * > params1a;
    if (m_fullFormulationAndRoutes.size() > 0) {
        for (const auto & f : m_fullFormulationAndRoutes) {
            const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, f->getFormulationAndRoute());
            if (params1 != nullptr) {
                params1a.push_back(params1);
                nTotal += params1->getNbParameters();
            }
        }
    }
    else {
        const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, m_formulation, m_route);
        if (params1 != nullptr) {
            nTotal += params1->getNbParameters();
        }
    }
    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteGroupId);
    if (params2 != nullptr) {
        nTotal += params2->getNbParameters();
    }
    return (m_index >= nTotal);
}

typedef struct {
    std::string id;
    bool isVariable;
} ddd;

const ParameterDefinition* ParameterDefinitionIterator::operator*()
{
    std::vector<const ParameterSetDefinition * > params1a;

    if (m_fullFormulationAndRoutes.size() > 0) {
        for (const auto & f : m_fullFormulationAndRoutes) {
            const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, f->getFormulationAndRoute());
            if (params1 != nullptr) {
                params1a.push_back(params1);
            }
        }
    }
    else {
        const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, m_formulation, m_route);
        if (params1 != nullptr) {
            params1a.push_back(params1);
        }
    }


    //const ParameterSetDefinition* params1 = m_model.getAbsorptionParameters(m_analyteGroupId, m_formulation, m_route);
    const ParameterSetDefinition* params2 = m_model.getDispositionParameters(m_analyteGroupId);

    std::vector<ddd> vector;

    // Check that there are absorption parameters (not the case for every model)
    /*if (params1 != nullptr) {
        for (size_t i = 0;i < params1->getNbParameters(); i++) {
            vector.push_back({params1->getParameter(i)->getId(), params1->getParameter(i)->isVariable()});
        }
    }*/

    // Check that there are absorption parameters (not the case for every model)
    for(const auto & params : params1a) {
        for (size_t i = 0;i < params->getNbParameters(); i++) {
            vector.push_back({params->getParameter(i)->getId(), params->getParameter(i)->isVariable()});
        }
    }

    for (size_t i = 0;i < params2->getNbParameters(); i++) {
        vector.push_back({params2->getParameter(i)->getId(), params2->getParameter(i)->isVariable()});
    }

    std::sort(vector.begin(), vector.end(), [&] (const ddd v1, const ddd v2) {
        if (v1.isVariable && !v2.isVariable)
            return true;
        if (!v1.isVariable && v2.isVariable)
            return false;
        return v1.id < v2.id;
    });

    std::string curId = vector[m_index].id;

    for(const auto & params : params1a) {
        for (size_t i = 0;i < params->getNbParameters(); i++) {
            if (params->getParameter(i)->getId() == curId)
                return params->getParameter(i);
        }
    }
/*
    if (params1 != nullptr) {
        for (size_t i = 0;i < params1->getNbParameters(); i++) {
            if (params1->getParameter(i)->getId() == curId)
                return params1->getParameter(i);
        }
    }
*/    for (size_t i = 0;i < params2->getNbParameters(); i++) {
        if (params2->getParameter(i)->getId() == curId)
            return params2->getParameter(i);
    }

    assert (false);


/*
    if (params1 != nullptr) {
        if (index < params1->getNbParameters()) {
            return params1->getParameter(index);
        }
        index -= params1->getNbParameters();
    }    
    if (params2 != nullptr && index < params2->getNbParameters()) {
        return params2->getParameter(index);
    }
            */
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
