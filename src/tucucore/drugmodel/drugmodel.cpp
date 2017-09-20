
/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucore/drugmodel/drugmodel.h"
#include "tucucore/covariate.h"

namespace Tucuxi {
namespace Core {

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

void DrugModel::addFormulationAndRoute(std::unique_ptr<FormulationAndRoute> _formulationAndRoute)
{
    m_formulationAndRoutes.push_back(std::move(_formulationAndRoute));
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
