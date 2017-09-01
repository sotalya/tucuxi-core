/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucore/covariate.h"
#include "tucucore/parameter.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/target.h"

namespace Tucuxi {
namespace Core {


class Formulation
{

};

class Route
{

};

// RouteOfAdministration shall be called AbsorptionModel

struct FormulationAndRoute
{
    Formulation m_formulation;
    Route m_route;
    RouteOfAdministration m_absorptionModel;

};

class Correlation
{
    Value m_correlation;
    std::vector<int> m_parameterId[2];
};

typedef std::vector<Correlation> Correlations;

struct AbsorptionParameters
{
    ParameterDefinitions m_parameters;
    Correlations m_correlations;
};


class DrugModel
{
public:
    DrugModel();    

    const Covariates& getCovariates() const;
    const ParameterDefinitions& getEliminationParameters() const { return m_eliminationParameters; }
    const IResidualErrorModel& getResidualErrorModel() const { return *m_residualErrorModel; }

    ///
    /// \brief getParameters
    /// Returns a full set of parameters, adding the corresponding absorption parameters to the
    /// elimination parameters
    /// \param _formulationAndRoute
    /// \return
    ///
    const ParameterDefinitions& getParameters(FormulationAndRoute _formulationAndRoute) const;

private:
    ParameterDefinitions m_eliminationParameters;
    std::map<FormulationAndRoute, AbsorptionParameters> absorptionParameters;

    Covariates m_covariates;
    IResidualErrorModel *m_residualErrorModel;
    Targets m_targets;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
