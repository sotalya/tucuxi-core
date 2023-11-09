//@@license@@

#ifndef TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H
#define TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H

#include <string>
#include <vector>

#include "tucucommon/unit.h"

#include "tucucore/definitions.h"
#include "tucucore/drugmodel/activesubstance.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/validdose.h"
#include "tucucore/drugmodel/validduration.h"
#include "tucucore/invariants.h"

namespace Tucuxi {
namespace Core {

class DrugModelChecker;

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

class StandardTreatment
{
public:
    StandardTreatment(bool _isFixedDuration = false, Value _duration = 0.0, TucuUnit _unit = TucuUnit("h"))
        : m_isFixedDuration(_isFixedDuration), m_duration(_duration), m_unit(std::move(_unit))
    {
    }

    bool getIsFixedDuration() const
    {
        return m_isFixedDuration;
    }
    //Tucuxi::Common::Duration getDuration() const { return m_duration;}
    Value getDuration() const
    {
        return m_duration;
    }
    TucuUnit getUnit() const
    {
        return m_unit;
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_STANDARDTREATMENT_0001,
                       ((!m_isFixedDuration) || (m_duration > 0.0)),
                       "A standard treatment duration is negative");
               INVARIANT(
                       Invariants::INV_STANDARDTREATMENT_0002,
                       ((!m_isFixedDuration)
                        || (Common::UnitManager::isOfType<Common::UnitManager::UnitType::Time>(m_unit))),
                       "A standard treatment duration unit is not correct");)

protected:
    bool m_isFixedDuration;
    Value m_duration;
    TucuUnit m_unit;
};

class AnalyteSetToAbsorptionAssociation
{
public:
    AnalyteSetToAbsorptionAssociation(const AnalyteSet& _analyteSet) : m_analyteSet(_analyteSet) {}

    void setAbsorptionModel(AbsorptionModel _absorptionModel)
    {
        m_absorptionModel = _absorptionModel;
    }
    void setAbsorptionParameters(std::unique_ptr<ParameterSetDefinition> _parameters)
    {
        m_absorptionParameters = std::move(_parameters);
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_ANALYTESETTOABSORPTIONASSOCIATION_0001,
                       (m_absorptionModel != AbsorptionModel::Undefined),
                       "An absorption model is undefined in an association");
               INVARIANT(
                       Invariants::INV_ANALYTESETTOABSORPTIONASSOCIATION_0002,
                       (m_absorptionParameters->checkInvariants()),
                       "An association of parameters to absorption model has an error");)

protected:
    std::unique_ptr<ParameterSetDefinition> m_absorptionParameters{nullptr};
    const AnalyteSet& m_analyteSet;

    ///
    /// \brief m_absorptionModel Computation absorption model
    /// This variable is an enum class and is closely related to the available Pk models implemented in the software
    AbsorptionModel m_absorptionModel{AbsorptionModel::Undefined};

    friend class FullFormulationAndRoute;
};

class AnalyteConversion
{
public:
    AnalyteConversion(AnalyteId _analyteId, Value _factor) : m_analyteId(std::move(_analyteId)), m_factor(_factor) {}

    AnalyteId getAnalyteId() const
    {
        return m_analyteId;
    }

    Value getFactor() const
    {
        return m_factor;
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_ANALYTECONVERSION_0001,
                       (m_factor >= 0.0),
                       "A factor in an analyte conversion is negative");
               INVARIANT(
                       Invariants::INV_ANALYTECONVERSION_0002,
                       (!m_analyteId.empty()),
                       "An analyte conversion does not have any analyte Id");)

protected:
    AnalyteId m_analyteId;
    Value m_factor;
};


class FormulationAndRoute
{
public:
    // Construction for testing purpose
    FormulationAndRoute(AbsorptionModel _absorptionModel)
        : m_formulation(Formulation::Undefined), m_route(AdministrationRoute::Undefined),
          m_absorptionModel(_absorptionModel), m_administrationName("")
    {
    }

    FormulationAndRoute(
            Formulation _formulation,
            AdministrationRoute _route,
            AbsorptionModel _absorptionModel,
            std::string _administrationName = "")
        : m_formulation(_formulation), m_route(_route), m_absorptionModel(_absorptionModel),
          m_administrationName(std::move(_administrationName))
    {
    }


    Formulation getFormulation() const
    {
        return m_formulation;
    }

    AdministrationRoute getAdministrationRoute() const
    {
        return m_route;
    }

    AbsorptionModel getAbsorptionModel() const
    {
        return m_absorptionModel;
    }

    std::string getAdministrationName() const
    {
        return m_administrationName;
    }
/*
    friend bool operator==(const FormulationAndRoute& _v1, const FormulationAndRoute& _v2)
    {
        return (_v1.m_absorptionModel == _v2.m_absorptionModel) && (_v1.m_route == _v2.m_route);
        return (_v1.m_absorptionModel == _v2.m_absorptionModel) && (_v1.m_route == _v2.m_route)
               && (_v1.m_formulation == _v2.m_formulation);
    }
  */

    bool operator==(const FormulationAndRoute& _v2) const
    {
        return (m_absorptionModel == _v2.m_absorptionModel) && (m_route == _v2.m_route)
               && (m_formulation == _v2.m_formulation)
                && (this->m_administrationName == _v2.m_administrationName);
        return (m_absorptionModel == _v2.m_absorptionModel) && (m_route == _v2.m_route);
        return (m_absorptionModel == _v2.m_absorptionModel) && (m_route == _v2.m_route)
               && (m_formulation == _v2.m_formulation);
    }

    bool isCompatible(const FormulationAndRoute& _v2) const
    {
        return (m_absorptionModel == _v2.m_absorptionModel) && (m_route == _v2.m_route);
        return (m_absorptionModel == _v2.m_absorptionModel) && (m_route == _v2.m_route)
                && (m_formulation == _v2.m_formulation);
    }

    /// \brief Is the duration smaller?
    bool operator<(const FormulationAndRoute& _f) const
    {
        if (m_formulation < _f.m_formulation) {
            return true;
        } else if (m_formulation > _f.m_formulation){
            return false;
        }
        if (m_route < _f.m_route) {
            return true;
        }else if (m_route > _f.m_route){
            return false;
        }
        if (m_absorptionModel < _f.m_absorptionModel) {
            return true;
        }else if (m_absorptionModel > _f.m_absorptionModel){
            return false;
        }
        if (m_administrationName < _f.m_administrationName) {
            return true;
        }else if (m_administrationName > _f.m_administrationName){
            return false;
        }
        return false;
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_FORMULATIONANDROUTE_0001,
                       (m_formulation != Formulation::Undefined),
                       "A formulation and route has no formulation defined");
               INVARIANT(
                       Invariants::INV_FORMULATIONANDROUTE_0001,
                       (m_route != AdministrationRoute::Undefined),
                       "A formulation and route has an undefined route of administration");
               INVARIANT(
                       Invariants::INV_FORMULATIONANDROUTE_0001,
                       (m_absorptionModel != AbsorptionModel::Undefined),
                       "A formulation and route has an undefined absorption model");
               INVARIANT(
                       Invariants::INV_FORMULATIONANDROUTE_0001,
                       (!m_administrationName.empty()),
                       "A formulation and route has an empty administration name");)

protected:
    /// Formulation, based on an Enum type
    Formulation m_formulation;

    /// Route of administration, based on an Enum type
    AdministrationRoute m_route;
    ///
    /// \brief m_absorptionModel Computation absorption model
    /// This variable is an enum class and is closely related to the available Pk models implemented in the software
    AbsorptionModel m_absorptionModel;

    /// Administration name, as a free text field
    std::string m_administrationName;
};

std::vector<FormulationAndRoute> mergeFormulationAndRouteList(
        const std::vector<FormulationAndRoute>& _v1, const std::vector<FormulationAndRoute>& _v2);



class FullFormulationAndRoute
{
public:
    FullFormulationAndRoute(const FormulationAndRoute& _specs, std::string _id)
        : m_id(std::move(_id)), m_specs(_specs), m_loadingDoseRecommended(true), m_restPeriodRecommended(true)
    {
    }

    void setValidDoses(std::unique_ptr<ValidDoses> _validDoses)
    {
        m_validDoses = std::move(_validDoses);
    }
    void setValidIntervals(std::unique_ptr<ValidDurations> _validIntervals)
    {
        m_validIntervals = std::move(_validIntervals);
    }
    void setValidInfusionTimes(std::unique_ptr<ValidDurations> _validInfusionTimes)
    {
        m_validInfusionTimes = std::move(_validInfusionTimes);
    }
    void addAssociation(std::unique_ptr<AnalyteSetToAbsorptionAssociation> _association)
    {
        m_associations.push_back(std::move(_association));
    }

    void setStandardTreatment(std::unique_ptr<StandardTreatment> _standardTreatment)
    {
        m_standardTreatment = std::move(_standardTreatment);
    }

    void addAnalyteConversion(std::unique_ptr<AnalyteConversion> _analyteConversion)
    {
        m_analyteConversions.push_back(std::move(_analyteConversion));
    }
    const std::vector<std::unique_ptr<AnalyteConversion> >& getAnalyteConversions() const
    {
        return m_analyteConversions;
    }
    const AnalyteConversion* getAnalyteConversion(const AnalyteId& _analyteId) const
    {
        for (const auto& analyteConversion : m_analyteConversions) {
            if (analyteConversion->getAnalyteId() == _analyteId) {
                return analyteConversion.get();
            }
        }
        return nullptr;
    }

    const ParameterSetDefinition* getParameterDefinitions(const AnalyteGroupId& _analyteGroupId) const;

    AbsorptionModel getAbsorptionModel(const AnalyteGroupId& _analyteGroupId) const;


    std::string getId() const
    {
        return m_id;
    }

    const ValidDoses* getValidDoses() const
    {
        return m_validDoses.get();
    }

    const ValidDurations* getValidIntervals() const
    {
        return m_validIntervals.get();
    }
    const ValidDurations* getValidInfusionTimes() const
    {
        return m_validInfusionTimes.get();
    }
    const FormulationAndRoute& getFormulationAndRoute() const
    {
        return m_specs;
    }

    const StandardTreatment* getStandardTreatment() const
    {
        return m_standardTreatment.get();
    }

    void setLoadingDoseRecommended(bool _recommend)
    {
        m_loadingDoseRecommended = _recommend;
    }

    void setRestPeriodRecommended(bool _recommend)
    {
        m_restPeriodRecommended = _recommend;
    }

    bool isLoadingDoseRecommended() const
    {
        return m_loadingDoseRecommended;
    }

    bool isRestPeriodRecommended() const
    {
        return m_restPeriodRecommended;
    }

    INVARIANTS(INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0001,
                       (!m_id.empty()),
                       "A formulation and route has no Id");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0002,
                       (m_validDoses != nullptr),
                       "A formulation and route ha no valid doses");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0003,
                       (m_validDoses->checkInvariants()),
                       "A formulation and route has an error in its valid doses");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0004,
                       (m_validIntervals != nullptr),
                       "A formulation and route has no valid intervals");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0005,
                       (m_validIntervals->checkInvariants()),
                       "A formulation and route has an error in its valid intervals");
               // INVARIANT(Invariants::INV_FULLFORMULATIONANDROUTE_0006, (m_validInfusionTimes != nullptr))
               LAMBDA_INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0007,
                       {
                           bool ok = true;
                           if (m_validInfusionTimes != nullptr) {
                               ok &= m_validInfusionTimes->checkInvariants();
                           }
                           return ok;
                       },
                       "There is an error in the valid infusion times");
               LAMBDA_INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0008,
                       {
                           bool ok = true;
                           if (m_standardTreatment != nullptr) {
                               ok &= m_standardTreatment->checkInvariants();
                           }
                           return ok;
                       },
                       "There is an error in the standard treatment");
               // To check : do we need at least one association?
               LAMBDA_INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0009,
                       {
                           bool ok = true;
                           for (size_t i = 0; i < m_associations.size(); i++) {
                               ok &= m_associations[i]->checkInvariants();
                           }
                           return ok;
                       },
                       "There is an error in an absorption model-parameters association");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0010,
                       (!m_analyteConversions.empty()),
                       "A formulation and route has no analyte conversion");
               LAMBDA_INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0011,
                       {
                           bool ok = true;
                           for (size_t i = 0; i < m_analyteConversions.size(); i++) {
                               ok &= m_analyteConversions[i] != nullptr;
                           }
                           return ok;
                       },
                       "There is an error in an analyte conversion. It is nullptr");
               LAMBDA_INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0012,
                       {
                           bool ok = true;
                           for (size_t i = 0; i < m_analyteConversions.size(); i++) {
                               ok &= m_analyteConversions[i]->checkInvariants();
                           }
                           return ok;
                       },
                       "There is an error in an analyte conversion");
               INVARIANT(
                       Invariants::INV_FULLFORMULATIONANDROUTE_0013,
                       (m_specs.checkInvariants()),
                       "A formulation and route has an error in its specific formulation and route");)

protected:
    /// A unique Id, useful when a DrugModel embeds more than one Formulation
    std::string m_id;

    FormulationAndRoute m_specs;

    std::vector<std::unique_ptr<AnalyteConversion> > m_analyteConversions;

    std::unique_ptr<ValidDoses> m_validDoses;
    std::unique_ptr<ValidDurations> m_validIntervals;
    std::unique_ptr<ValidDurations> m_validInfusionTimes;

    std::vector<std::unique_ptr<AnalyteSetToAbsorptionAssociation> > m_associations;

    /// \brief m_loadingDoseRecommended indicates if a loading dose should be proposed if the dosage is too low
    bool m_loadingDoseRecommended;

    /// \brief m_restPeriodRecommended indicates if a rest period should be proposed if the dosage is too high
    bool m_restPeriodRecommended;

    std::unique_ptr<StandardTreatment> m_standardTreatment;

    friend class FormulationAndRoutes;
    friend class DrugModelChecker;
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
    void add(std::unique_ptr<FullFormulationAndRoute> _far, bool _isDefault = false);

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
    Iterator begin() const
    {
        return m_fars.begin();
    }

    ///
    /// \brief end returns an iterator on the end of the list
    /// \return The iterator on the end of the list
    ///
    Iterator end() const
    {
        return m_fars.end();
    }



    INVARIANTS(
            INVARIANT(Invariants::INV_FORMULATIONANDROUTE_0001, (!m_fars.empty()), "There is no formulation and route");
            LAMBDA_INVARIANT(
                    Invariants::INV_FORMULATIONANDROUTE_0002,
                    {
                        bool ok = true;
                        for (size_t i = 0; i < m_fars.size(); i++) {
                            ok &= m_fars[i]->checkInvariants();
                        }
                        return ok;
                    },
                    "There is an error in a formulation and route");
            INVARIANT(
                    Invariants::INV_FORMULATIONANDROUTE_0003,
                    (m_defaultIndex < m_fars.size()),
                    "The default formulation and route does not refer to an existing one");)

private:
    //! Vector of formulation and routes
    std::vector<std::unique_ptr<FullFormulationAndRoute> > m_fars;

    //! Index of the default formulation and route. 0 by default
    std::size_t m_defaultIndex;

    friend DrugModelChecker;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_TUCUCORE_FORMULATIONANDROUTE_H
