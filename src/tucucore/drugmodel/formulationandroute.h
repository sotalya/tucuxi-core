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
#include "tucucore/drugmodel/validdose.h"
#include "tucucore/drugmodel/validduration.h"


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


class AnalyteSetToAbsorptionAssociation
{
public:
    AnalyteSetToAbsorptionAssociation(const AnalyteSet & _analyteSet) :
        m_analyteSet(_analyteSet)
    {}

    void setAbsorptionModel(AbsorptionModel _absorptionModel) { m_absorptionModel = _absorptionModel;}
    void setAbsorptionParameters(std::unique_ptr<ParameterSetDefinition> _parameters) {m_absorptionParameters = std::move(_parameters);}

protected:
    std::unique_ptr<ParameterSetDefinition> m_absorptionParameters;
    const AnalyteSet &m_analyteSet;
    AbsorptionModel m_absorptionModel;

    friend class FormulationAndRoute;
};

class ForumulationAndRoutes;

class FormulationAndRoute
{
public:
    FormulationAndRoute(const Formulation& _formulation, const Route _route) 
        : m_formulation(_formulation), m_route(_route)
    {}

    void setValidDoses(std::unique_ptr<ValidDoses> _validDoses) {m_validDoses = std::move(_validDoses);}
    void setValidIntervals(std::unique_ptr<ValidDurations> _validIntervals) {m_validIntervals = std::move(_validIntervals);}
    void setValidInfusionTimes(std::unique_ptr<ValidDurations> _validInfusionTimes) {m_validInfusionTimes = std::move(_validInfusionTimes);}
    void addAssociation(std::unique_ptr< AnalyteSetToAbsorptionAssociation > _association) {m_associations.push_back(std::move(_association));}

    const ParameterSetDefinition* getParameterDefinitions(const std::string &_analyteId) const;

protected:
    Formulation m_formulation;
    Route m_route;

    std::unique_ptr<ValidDoses> m_validDoses;
    std::unique_ptr<ValidDurations> m_validIntervals;
    std::unique_ptr<ValidDurations> m_validInfusionTimes;

    std::vector<std::unique_ptr< AnalyteSetToAbsorptionAssociation > > m_associations;    

    friend class FormulationAndRoutes;
};

class FormulationAndRoutes
{
public:
    void add(std::unique_ptr<FormulationAndRoute> _far);
    const FormulationAndRoute* get(const Formulation& _formulation, const Route _route) const;

private:
    std::vector<std::unique_ptr<FormulationAndRoute> > m_fars;
};

} // namespace Core
} // namespace Tucuxi


#endif // FORMULATIONANDROUTE_H
