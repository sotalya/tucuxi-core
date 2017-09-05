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



class Analyte
{
public:

    std::string m_name;
    std::string m_analyteId;

};

class ActiveSubstance
{
public:

    std::string m_pkModelId;
    std::vector<Analyte> m_analytes;
};


typedef std::string Formulation;


enum class Route
{
    Intramuscular,
    IntravenousBolus,
    IntravenousDrip,
    Nasal,
    Oral,
    Rectal,
    Subcutaneous,
    Sublingual,
    Transdermal,
    Vaginal
};


class Correlation
{
    Value m_correlation;
    std::vector<int> m_parameterId[2];
};

typedef std::vector<Correlation> Correlations;



class AbsorptionParameters
{
public:

    ParameterDefinitions m_parameters;
    Correlations m_correlations;
};


// RouteOfAdministration shall be called AbsorptionModel

class FormulationAndRoute
{
public:

    Formulation m_formulation;
    Route m_route;
    RouteOfAdministration m_absorptionModel;

    std::vector<ActiveSubstance* > m_activeSubstance;

    AbsorptionParameters m_absorptionParameters;

};


class DrugModel
{
public:
    DrugModel();    

    const Covariates& getCovariates() const;
    const ParameterDefinitions& getDispositionParameters() const { return m_dispositionParameters; }
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

    std::vector<FormulationAndRoute* > m_formulationAndRoutes;

    ParameterDefinitions m_dispositionParameters;

    Correlations m_dispositionParametersCorrelations;


    Covariates m_covariates;
    IResidualErrorModel *m_residualErrorModel;
    Targets m_targets;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
