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

    void addParameter(ParameterDefinition *_parameter) { m_parameters.push_back(std::unique_ptr<ParameterDefinition>(_parameter));}
    void setCorrelations(Correlations _correlations) { m_correlations = _correlations;}

protected:

    ParameterDefinitions m_parameters;
    Correlations m_correlations;

};


class FormulationAndRoute
{
public:

    void setFormulation(Formulation _formulation) { m_formulation = _formulation;}
    void setRoute(Route _route){  m_route = _route;}
    void setAbsorptionModel(AbsorptionModel _absorptionModel) { m_absorptionModel = _absorptionModel;}
    void addActiveSubstance(ActiveSubstance *_activeSubstance) { m_activeSubstances.push_back(_activeSubstance);}
    void addAbsorptionParameter(ParameterDefinition *_parameter) {m_absorptionParameters.addParameter(_parameter);}
    void setCorrelations(Correlations _correlations) { m_absorptionParameters.setCorrelations(_correlations);}

protected:

    Formulation m_formulation;
    Route m_route;
    AbsorptionModel m_absorptionModel;

    std::vector<ActiveSubstance* > m_activeSubstances;

    AbsorptionParameters m_absorptionParameters;

};

typedef std::vector<std::unique_ptr<FormulationAndRoute> > FormulationAndRoutes;


class DrugModelDomain
{
public:
    DrugModelDomain(std::unique_ptr<Operation> _constraint) :
        m_constraint(std::move(_constraint)) {}

    const Operation &getConstraint() { return *m_constraint;}

protected:
    std::unique_ptr<Operation> m_constraint;
};


class DrugModel
{
public:
    DrugModel();

    const CovariateDefinitions &getCovariates() const;
    const ParameterDefinitions& getDispositionParameters() const { return m_dispositionParameters; }
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
        m_dispositionParameters.push_back(std::unique_ptr<ParameterDefinition>(_definition));
    }

    void addFormulationAndRoute(FormulationAndRoute *_formulationAndRoute) { m_formulationAndRoutes.push_back(std::unique_ptr<FormulationAndRoute>(_formulationAndRoute));}

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate) { m_covariates.push_back(std::move(_covariate));}


    void addTarget(TargetDefinition *_target) { m_targets.push_back(std::unique_ptr<TargetDefinition>(_target));}

    void setResidualErrorModel(std::unique_ptr<IResidualErrorModel>& _residualErrorModel) { m_residualErrorModel = std::move(_residualErrorModel);}

    void setDomain(std::unique_ptr<DrugModelDomain>& _domain) {m_domain = std::move(_domain);}

private:

    std::unique_ptr<DrugModelDomain> m_domain;

    FormulationAndRoutes m_formulationAndRoutes;

    ParameterDefinitions m_dispositionParameters;

    Correlations m_dispositionParametersCorrelations;


    CovariateDefinitions m_covariates;
    std::unique_ptr<IResidualErrorModel> m_residualErrorModel;
    TargetDefinitions m_targets;


};

}
}

#endif // TUCUXI_CORE_DRUGMODEL_H
