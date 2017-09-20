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

    void setTimeToSteadyState(Tucuxi::Common::Duration _time);

    Tucuxi::Common::Duration getTimeToSteadyState() const;

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate);

    const CovariateDefinitions& getCovariates() const;


    const FormulationAndRoutes& getFormulationAndRoutes() const;

    ///
    /// \brief getParameters
    /// Returns a full set of parameters, adding the corresponding absorption parameters to the
    /// elimination parameters
    /// \param _formulationAndRoute
    /// \return
    ///
    //const ParameterDefinitions& getParameters(FormulationAndRoute _formulationAndRoute) const;

    void addFormulationAndRoute(std::unique_ptr<FormulationAndRoute> _formulationAndRoute);




    void setDomain(std::unique_ptr<DrugModelDomain> _domain);

    void setAnalyteSet(std::unique_ptr<AnalyteSet> _analyteSet);


private:

    Tucuxi::Common::Duration m_timeToSteadyState;

    std::vector< std::unique_ptr<AnalyteSet> > m_analyteSets;

    std::unique_ptr<DrugModelDomain> m_domain;

    FormulationAndRoutes m_formulationAndRoutes;

    CovariateDefinitions m_covariates;

    InterParameterSetCorrelations m_interParameterSetCorrelations;

};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
