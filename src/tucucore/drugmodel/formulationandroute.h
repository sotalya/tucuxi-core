/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef FORMULATIONANDROUTE_H
#define FORMULATIONANDROUTE_H

#include <string>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/activesubstance.h"


namespace Tucuxi {
namespace Core {


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

}
}


#endif // FORMULATIONANDROUTE_H
