/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucore/covariate.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/drugmodel/targetdefinition.h"


#include "tucucore/drugmodel/activesubstance.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugmodel/formulationandroute.h"

namespace Tucuxi {
namespace Core {






class DrugModel
{
public:
    DrugModel();

    const CovariateDefinitions &getCovariates() const;
    const ParameterDefinitions& getDispositionParameters() const { ParameterDefinitions res; return res;/*return m_dispositionParameters;*/ }
    const IResidualErrorModel& getResidualErrorModel() const { return *m_residualErrorModel; }
    const FormulationAndRoutes& getFormulationAndRoutes() const { return m_formulationAndRoutes; }

    ///
    /// \brief getParameters
    /// Returns a full set of parameters, adding the corresponding absorption parameters to the
    /// elimination parameters
    /// \param _formulationAndRoute
    /// \return
    ///
    const ParameterDefinitions& getParameters(FormulationAndRoute _formulationAndRoute) const;

    void addDispositionParameter(ParameterDefinition *_definition) {
    //    m_dispositionParameters.push_back(std::unique_ptr<ParameterDefinition>(_definition));
    }

    void addFormulationAndRoute(FormulationAndRoute *_formulationAndRoute) { m_formulationAndRoutes.push_back(std::unique_ptr<FormulationAndRoute>(_formulationAndRoute));}

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate) { m_covariates.push_back(std::move(_covariate));}


    void addTarget(std::unique_ptr<TargetDefinition>& _target) { m_targets.push_back(std::move(_target));}

    void setResidualErrorModel(std::unique_ptr<IResidualErrorModel>& _residualErrorModel) { m_residualErrorModel = std::move(_residualErrorModel);}

    void setDomain(std::unique_ptr<DrugModelDomain>& _domain) {m_domain = std::move(_domain);}

private:

    std::unique_ptr<DrugModelDomain> m_domain;

    FormulationAndRoutes m_formulationAndRoutes;

    ParameterSetDefinition m_dispositionParameters;

//    Correlations m_dispositionParametersCorrelations;


    CovariateDefinitions m_covariates;
    std::unique_ptr<IResidualErrorModel> m_residualErrorModel;
    TargetDefinitions m_targets;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
