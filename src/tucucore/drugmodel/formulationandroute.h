/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H
#define TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H

#include <string>
#include <vector>

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/activesubstance.h"
#include "tucucore/drugmodel/validdose.h"
#include "tucucore/drugmodel/validduration.h"


namespace Tucuxi {
namespace Core {


enum class Formulation
{
    Undefined,
    ParenteralSolution,
    OralSolution,
    Test
};


enum class AdministrationRoute
{

    Undefined,
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

    void setRouteModel(RouteModel _absorptionModel) { m_routeModel = _absorptionModel;}
    void setAbsorptionParameters(std::unique_ptr<ParameterSetDefinition> _parameters) {m_absorptionParameters = std::move(_parameters);}

protected:
    std::unique_ptr<ParameterSetDefinition> m_absorptionParameters;
    const AnalyteSet &m_analyteSet;

    ///
    /// \brief m_absorptionModel Computation absorption model
    /// This variable is an enum class and is closely related to the available Pk models implemented in the software
    RouteModel m_routeModel;

    friend class FullFormulationAndRoute;
};

class ForumulationAndRoutes;

class FormulationAndRoute
{
public:

    FormulationAndRoute(
            Formulation _formulation,
            AdministrationRoute _route,
            RouteModel _absorptionModel,
            std::string _administrationName = "") :
        m_formulation(_formulation), m_route(_route), m_absorptionModel(_absorptionModel),
        m_administrationName(_administrationName)
    {}


    Formulation getFormulation() const { return m_formulation;}

    AdministrationRoute getAdministrationRoute() const { return m_route;}

    RouteModel getRouteModel() const { return m_absorptionModel;}

    std::string getAdministrationName() const { return m_administrationName;}

    friend bool operator==(const FormulationAndRoute& v1, const FormulationAndRoute& v2)
    {
        return (v1.m_absorptionModel == v2.m_absorptionModel) &&
                (v1.m_route == v2.m_route);
    }

protected:


    /// Formulation, based on an Enum type
    Formulation m_formulation;

    /// Route of administration, based on an Enum type
    AdministrationRoute m_route;
    ///
    /// \brief m_absorptionModel Computation absorption model
    /// This variable is an enum class and is closely related to the available Pk models implemented in the software
    RouteModel m_absorptionModel;

    /// Administration name, as a free text field
    std::string m_administrationName;
};


class FullFormulationAndRoute
{
public:

    FullFormulationAndRoute(const FormulationAndRoute& _specs, std::string _id)
        : m_id(_id), m_specs(_specs)
    {}

    void setValidDoses(std::unique_ptr<ValidDoses> _validDoses) {m_validDoses = std::move(_validDoses);}
    void setValidIntervals(std::unique_ptr<ValidDurations> _validIntervals) {m_validIntervals = std::move(_validIntervals);}
    void setValidInfusionTimes(std::unique_ptr<ValidDurations> _validInfusionTimes) {m_validInfusionTimes = std::move(_validInfusionTimes);}
    void addAssociation(std::unique_ptr< AnalyteSetToAbsorptionAssociation > _association) {m_associations.push_back(std::move(_association));}

    const ParameterSetDefinition* getParameterDefinitions(const std::string &_analyteId) const;

    std::string getId() const { return m_id;}

    const ValidDoses* getValidDoses() const { return m_validDoses.get();}

    const ValidDurations* getValidIntervals() const { return m_validIntervals.get();}
    const ValidDurations* getValidInfusionTimes() const { return m_validInfusionTimes.get();}
    const FormulationAndRoute& getFormulationAndRoute() const { return m_specs;}

protected:

    /// A unique Id, useful when a DrugModel embeds more than one Formulation
    std::string m_id;

    FormulationAndRoute m_specs;

    std::unique_ptr<ValidDoses> m_validDoses;
    std::unique_ptr<ValidDurations> m_validIntervals;
    std::unique_ptr<ValidDurations> m_validInfusionTimes;

    std::vector<std::unique_ptr< AnalyteSetToAbsorptionAssociation > > m_associations;    

    friend class FormulationAndRoutes;
};

///
/// \brief The FormulationAndRoutes class
/// This class embeds a vector of FormulationAndRoute objects. It also offers a mechanism to identify the default to be used.
///
class FormulationAndRoutes
{
public:

    ///
    /// \brief FormulationAndRoutes Empty constructor
    ///
    FormulationAndRoutes();

    ///
    /// \brief add Adds a new formulation and route to the set
    /// \param _far A unique pointer to the formulation and route
    /// \param isDefault If true, then the newly added is the default formulation and route, else not.
    /// If there is a single formulation and route in the set, then the single one is the default.
    ///
    void add(std::unique_ptr<FullFormulationAndRoute> _far, bool isDefault = false);

    ///
    /// \brief get Get a formulation and route object based on the formulation and the route
    /// \param _formulation Formulation to look for
    /// \param _route Route to look for
    /// \return A pointer to a FormulationAndRoute, nullptr if not in the set
    ///
    const FullFormulationAndRoute* get(const Formulation& _formulation, AdministrationRoute _route) const;

    ///
    /// \brief get Get a formulation and route object based on the formulation and the route
    /// \param _formulation Formulation and route to look for
    /// \return A pointer to a FullFormulationAndRoute, nullptr if not in the set
    ///
    const FullFormulationAndRoute* get(const FormulationAndRoute& _formulation) const;

    ///
    /// \brief getDefault Get the default formulation and route object.
    /// \return  The default formulation and route, nullptr if the set is empty
    ///
    const FullFormulationAndRoute* getDefault() const;

    ///
    /// \brief getList Get a reference to the vector of formulation and routes
    /// \return A vector of Formulation and routes
    ///
    const std::vector<std::unique_ptr<FullFormulationAndRoute> >& getList() const;


    ///
    /// \brief Iterator Definition of an iterator for the FormulationAndRoute objects
    ///
    typedef std::vector<std::unique_ptr<FullFormulationAndRoute> >::const_iterator Iterator;

    ///
    /// \brief begin returns an iterator on the beginning of the list
    /// \return The iterator on the beginning of the list
    ///
    Iterator begin() const { return m_fars.begin(); }

    ///
    /// \brief end returns an iterator on the end of the list
    /// \return The iterator on the end of the list
    ///
    Iterator end() const { return m_fars.end(); }


private:

    //! Vector of formulation and routes
    std::vector<std::unique_ptr<FullFormulationAndRoute> > m_fars;

    //! Index of the default formulation and route. 0 by default
    std::size_t m_defaultIndex;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H
