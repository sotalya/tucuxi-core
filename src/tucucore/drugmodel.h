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
    void setName(std::string _name) { m_name = _name;}
    void setAnalyteId(std::string _analyteId) { m_analyteId = _analyteId;}

    std::string m_name;
    std::string m_analyteId;

};

class ActiveSubstance
{
public:

    void setPkModelId(std::string _pkModelId) { m_pkModelId = _pkModelId;}
    void addAnalyte(Analyte _analyte) { m_analytes.push_back(_analyte);}

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
    void setParameters(ParameterDefinitions _parameters) { m_parameters = _parameters;}
    void setCorrelations(Correlations _correlations) { m_correlations = _correlations;}

    ParameterDefinitions m_parameters;
    Correlations m_correlations;
};


// RouteOfAdministration shall be called AbsorptionModel

class FormulationAndRoute
{
public:

    void setAbsorptionParameters(AbsorptionParameters _absorptionParameters)
    {
        m_absorptionParameters = _absorptionParameters;
    }

    void setFormulation(Formulation _formulation) { m_formulation = _formulation;}
    void setRoute(Route _route){  m_route = _route;}
    void setAbsorptionModel(AbsorptionModel _absorptionModel) { m_absorptionModel = _absorptionModel;}
    void addActiveSubstance(ActiveSubstance *_activeSubstance) { m_activeSubstances.push_back(_activeSubstance);}

    Formulation m_formulation;
    Route m_route;
    AbsorptionModel m_absorptionModel;

    std::vector<ActiveSubstance* > m_activeSubstances;

    AbsorptionParameters m_absorptionParameters;

};


class DrugModel
{
public:
    DrugModel();

    const CovariateDefinitions &getCovariates() const;
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

    void setDispositionParameters(ParameterDefinitions &_definitions) {
        m_dispositionParameters = _definitions;
    }

    void addFormulationAndRoute(FormulationAndRoute *_formulationAndRoute) { m_formulationAndRoutes.push_back(_formulationAndRoute);}

    void setCovariates(CovariateDefinitions _covariates) { m_covariates = _covariates; }

    void setResidualErrorMoedl(IResidualErrorModel *_residualErrorModel) { m_residualErrorModel = _residualErrorModel;}

    void setTargets(TargetDefinitions _targets) { m_targets = _targets;}

private:

    std::vector<FormulationAndRoute* > m_formulationAndRoutes;

    ParameterDefinitions m_dispositionParameters;

    Correlations m_dispositionParametersCorrelations;


    CovariateDefinitions m_covariates;
    IResidualErrorModel *m_residualErrorModel;
    TargetDefinitions m_targets;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
