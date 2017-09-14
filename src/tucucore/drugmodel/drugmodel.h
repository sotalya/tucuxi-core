/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucore/covariate.h"
#include "tucucore/drugmodel/parameterdefinition.h"


//#include "tucucore/drugmodel/activesubstance.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugmodel/formulationandroute.h"

namespace Tucuxi {
namespace Core {




class DrugModel
{
public:
    DrugModel();

    const CovariateDefinitions &getCovariates() const;
    const FormulationAndRoutes& getFormulationAndRoutes() const { return m_formulationAndRoutes; }

    ///
    /// \brief getParameters
    /// Returns a full set of parameters, adding the corresponding absorption parameters to the
    /// elimination parameters
    /// \param _formulationAndRoute
    /// \return
    ///
    const ParameterDefinitions& getParameters(FormulationAndRoute _formulationAndRoute) const;

    void addFormulationAndRoute(std::unique_ptr<FormulationAndRoute> _formulationAndRoute) { m_formulationAndRoutes.push_back(std::move(_formulationAndRoute));}

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate) { m_covariates.push_back(std::move(_covariate));}



    void setDomain(std::unique_ptr<DrugModelDomain>& _domain) {m_domain = std::move(_domain);}

private:

    std::vector< std::unique_ptr<AnalyteSet > > m_analyteGroups;

    std::unique_ptr<DrugModelDomain> m_domain;

    FormulationAndRoutes m_formulationAndRoutes;


//    Correlations m_dispositionParametersCorrelations;


    CovariateDefinitions m_covariates;

    InterParameterSetCorrelations m_interParameterSetCorrelations;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
