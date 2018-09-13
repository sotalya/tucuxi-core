/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include "tucucommon/iterator.h"

#include "tucucore/covariateevent.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/timeconsiderations.h"

namespace Tucuxi {
namespace Core {

enum class Invariants {
    INV_0001 = 0,
    INV_0002
};

typedef std::vector< std::unique_ptr<AnalyteSet>> AnalyteSets;

class DrugModel;
class ParameterDefinitionIterator : public Tucuxi::Common::Iterator<const ParameterDefinition*>
{
public:
    ParameterDefinitionIterator(const DrugModel &_model, const std::string& _analyteId, const Formulation &_formulation, const AdministrationRoute _route)
        : m_model(_model), m_analyteId(_analyteId), m_formulation(_formulation), m_route(_route)
    {
    }
    ParameterDefinitionIterator(const ParameterDefinitionIterator& _right)
        : m_model(_right.m_model), m_analyteId(_right.m_analyteId), m_formulation(_right.m_formulation), m_route(_right.m_route), m_index(_right.m_index)
    {
    }
    const ParameterDefinition* operator*() override;
    Tucuxi::Common::Iterator<const ParameterDefinition*>& next() override
    {
        m_index++;
        return *this;
    }
    bool isDone() const override;
    void reset() override { m_index = 0; }

private:
    const DrugModel &m_model;
    const std::string m_analyteId;
    const Formulation m_formulation;
    const AdministrationRoute m_route;
    size_t m_index;
};

#define INVARIANT(invariant, expression) ok &= expression;
#define INVARIANTS(decl) public : bool checkInvariants() const {bool ok=true;decl;return ok;}
#define CHECKINVARIANTS checkInvariants()


class DrugModel
{

    INVARIANTS(
            INVARIANT(Invariants::INV_0001, (m_drugId.size() != 0))
            INVARIANT(Invariants::INV_0002, (m_drugModelId.size() != 0))
            )


public:
    DrugModel();

    void setDrugId(std::string _drugId) { m_drugId = _drugId;}
    void setDrugModelId(std::string _drugModelId) { m_drugModelId = _drugModelId;}

    void setTimeToSteadyState(Tucuxi::Common::Duration _time);

    Tucuxi::Common::Duration getTimeToSteadyState() const;

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate);

    const CovariateDefinitions& getCovariates() const;

    const FormulationAndRoutes& getFormulationAndRoutes() const;

    ParameterDefinitionIterator getParameterDefinitions(const std::string& _analyteId, const Formulation &_formulation, const AdministrationRoute _route) const
    {
        ParameterDefinitionIterator iterator(*this, _analyteId, _formulation, _route);
        return iterator;
    }

    ///
    /// \brief getParameters
    /// Returns a full set of parameters, adding the corresponding absorption parameters to the
    /// elimination parameters
    /// \param _formulationAndRoute
    /// \return
    ///
    //const ParameterDefinitions& getParameters(FormulationAndRoute _formulationAndRoute) const;

    void addFormulationAndRoute(std::unique_ptr<FullFormulationAndRoute> _formulationAndRoute, bool _isDefault = false);
    
    void setDomain(std::unique_ptr<DrugModelDomain> _domain);

    void addAnalyteSet(std::unique_ptr<AnalyteSet> _analyteSet);

    std::string getPkModelId() const {
        if (m_analyteSets.size() > 0) {
            return m_analyteSets.at(0)->getPkModelId();
        }
        return "";
    }

    void addActiveMoiety(std::unique_ptr<ActiveMoiety> _activeMoiety);

    const ActiveMoieties& getActiveMoieties() const { return m_activeMoieties;}

    void setTimeConsiderations(std::unique_ptr<TimeConsiderations> _timeConsiderations)
    {
        m_timeConsiderations = std::move(_timeConsiderations);
    }

    void setFormulationAndRoutes(std::unique_ptr<FormulationAndRoutes> _formulationAndRoutes)
    {
        m_formulationAndRoutes = std::move(_formulationAndRoutes);
    }

    const TimeConsiderations &getTimeConsiderations() const { return *m_timeConsiderations;}

private:


    const AnalyteSet* getAnalyteSet(const std::string &_analyteId) const {
        for (const std::unique_ptr<AnalyteSet> &set : m_analyteSets) {
            if (set->getId() == _analyteId) {
                return set.get();
            }
        }
        return nullptr;
    }

    const FullFormulationAndRoute* getFormulationAndRoute(const Formulation &_formulation, const AdministrationRoute _route) const {
        return m_formulationAndRoutes->get(_formulation, _route);
    }

    const ParameterSetDefinition* getAbsorptionParameters(const std::string &_analyteId, const Formulation &_formulation, const AdministrationRoute _route) const {
        const FullFormulationAndRoute* fr = getFormulationAndRoute(_formulation, _route);
        if (fr != nullptr) {
            return fr->getParameterDefinitions(_analyteId);
        }
        return nullptr;
    }

    const ParameterSetDefinition* getDispositionParameters(const std::string &_analyteId) const {
        TMP_UNUSED_PARAMETER(_analyteId);
        CHECKINVARIANTS;
        const AnalyteSet* pSet = getAnalyteSet("" /*_analyteId*/);
        if (pSet != nullptr) {
            return &pSet->getDispositionParameters();
        }
        return nullptr;
    }

private:

    std::string m_drugId;

    std::string m_drugModelId;

    Tucuxi::Common::Duration m_timeToSteadyState;

    AnalyteSets m_analyteSets;

    std::unique_ptr<DrugModelDomain> m_domain;

    std::unique_ptr<FormulationAndRoutes> m_formulationAndRoutes;

    CovariateDefinitions m_covariates;

    InterParameterSetCorrelations m_interParameterSetCorrelations;

    ActiveMoieties m_activeMoieties;

    std::unique_ptr<TimeConsiderations> m_timeConsiderations;

    friend ParameterDefinitionIterator;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGMODEL_H
