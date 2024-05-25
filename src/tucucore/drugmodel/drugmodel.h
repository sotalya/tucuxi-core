/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_DRUGMODEL_H
#define TUCUXI_CORE_DRUGMODEL_H

#include <utility>

#include "tucucommon/iterator.h"

#include "tucucore/drugmodel/activemoiety.h"
#include "tucucore/drugmodel/covariatedefinition.h"
#include "tucucore/drugmodel/drugmodeldomain.h"
#include "tucucore/drugmodel/drugmodelmetadata.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/drugmodel/timeconsiderations.h"
#include "tucucore/invariants.h"

struct TestConstantEliminationBolus;
struct TestPkAsymptotic;
struct TestMultiAnalytesSingleActiveMoiety;
struct TestMultiAnalytesMultiActiveMoieties;

#ifdef DRUGMODELTESTS
class Drugs2Manager;
#endif // DRUGMODELTESTS

namespace Tucuxi {
namespace Core {


typedef std::vector<std::unique_ptr<AnalyteSet>> AnalyteSets;

class DrugModel;
class DrugModelChecker;

///
/// \brief The ParameterDefinitionIterator class
/// This class is the only way to access Pk parameters from the DrugModel.
///     /// It ensures the parameters are ordered correctly:
/// - first the variable parameters, then the fixed parameters
/// - Within a categoy, alphabetical order is respected.
///
/// For instance, with Ka fixed, F fixed, CL variable, V variable, the iterator will give:
/// 1. CL
/// 2. V
/// 3. F
/// 4. Ka
///
class ParameterDefinitionIterator : public Tucuxi::Common::Iterator<const ParameterDefinition*>
{
public:
    ParameterDefinitionIterator(
            const DrugModel& _model,
            AnalyteGroupId _analyteGroupId,
            const Formulation& _formulation,
            const AdministrationRoute _route)
        : m_model(_model), m_analyteGroupId(std::move(_analyteGroupId)), m_formulation(_formulation), m_route(_route)
    {
        build();
    }

    ParameterDefinitionIterator(
            const DrugModel& _model,
            AnalyteGroupId _analyteGroupId,
            const std::vector<const FullFormulationAndRoute*>& _formulation)
        : m_model(_model), m_analyteGroupId(std::move(_analyteGroupId)), m_formulation(Formulation::Undefined),
          m_route(AdministrationRoute::Undefined), m_fullFormulationAndRoutes(_formulation)
    {
        build();
    }

    ParameterDefinitionIterator(
            const DrugModel& _model,
            AnalyteGroupId _analyteGroupId,
            const std::vector<FormulationAndRoute>& _formulation)
        : m_model(_model), m_analyteGroupId(std::move(_analyteGroupId)), m_formulation(Formulation::Undefined),
          m_route(AdministrationRoute::Undefined), m_formulationAndRoutes(_formulation)
    {
        build();
    }

    ParameterDefinitionIterator(const ParameterDefinitionIterator& _right)
        : m_model(_right.m_model), m_analyteGroupId(_right.m_analyteGroupId), m_formulation(_right.m_formulation),
          m_route(_right.m_route), m_fullFormulationAndRoutes(_right.m_fullFormulationAndRoutes),
          m_formulationAndRoutes(_right.m_formulationAndRoutes), m_index(_right.m_index)

    {
        build();
    }


    const ParameterDefinition* operator*() override;
    Tucuxi::Common::Iterator<const ParameterDefinition*>& next() override
    {
        m_index++;
        return *this;
    }
    bool isDone() const override;
    void reset() override
    {
        m_index = 0;
    }

private:
    void build();

    const DrugModel& m_model;
    const AnalyteGroupId m_analyteGroupId;
    const Formulation m_formulation;
    const AdministrationRoute m_route;
    const std::vector<const FullFormulationAndRoute*> m_fullFormulationAndRoutes;
    const std::vector<FormulationAndRoute> m_formulationAndRoutes;
    size_t m_index{0};
    size_t m_total{0};

    std::vector<const ParameterSetDefinition*> m_absorptionParameters;


    typedef struct
    {
        std::string id;  // NOLINT(readability-identifier-naming)
        bool isVariable; // NOLINT(readability-identifier-naming)
    } ParameterInfo;

    std::vector<ParameterInfo> m_parametersVector;
};

class DrugModel //Some modifications required for checking the structure of the drugmodel to find some issues in it
{

    INVARIANTS(
            INVARIANT(Invariants::INV_DRUGMODEL_0001, (!m_drugId.empty()), "A drug model has no drug Id");
            INVARIANT(Invariants::INV_DRUGMODEL_0002, (!m_drugModelId.empty()), "A drug model has no drug model Id");
            INVARIANT(Invariants::INV_DRUGMODEL_0003, (!m_analyteSets.empty()), "A drug model has no set of analytes");
            LAMBDA_INVARIANT(
                    Invariants::INV_DRUGMODEL_0004,
                    {
                        bool ok = true;
                        for (const auto& analyteSet : m_analyteSets) {
                            ok &= analyteSet->checkInvariants();
                        }
                        return ok;
                    },
                    "There is an error in an analyte group");
            INVARIANT(Invariants::INV_DRUGMODEL_0005, (m_domain != nullptr), "A drug model has no domain") INVARIANT(
                    Invariants::INV_DRUGMODEL_0006,
                    (m_domain->checkInvariants()),
                    "A drug model has an error in its domain");
            LAMBDA_INVARIANT(
                    Invariants::INV_DRUGMODEL_0007,
                    {
                        bool ok = true;
                        for (const auto& covariate : m_covariates) {
                            ok &= covariate->checkInvariants();
                        }
                        return ok;
                    },
                    "There is an error in a covariate definition");
            INVARIANT(
                    Invariants::INV_DRUGMODEL_0008,
                    (m_formulationAndRoutes != nullptr),
                    "A drug model has no formulation and route");
            INVARIANT(
                    Invariants::INV_DRUGMODEL_0009,
                    (m_formulationAndRoutes->checkInvariants()),
                    "There is an error in a drug model formulation and route");
            LAMBDA_INVARIANT(
                    Invariants::INV_DRUGMODEL_0010,
                    {
                        bool ok = true;
                        for (const auto& interParameterSetCorrelation : m_interParameterSetCorrelations) {
                            ok &= interParameterSetCorrelation.checkInvariants();
                        }
                        return ok;
                    },
                    "There is an error in an inter-parameter set correlation");
            INVARIANT(Invariants::INV_DRUGMODEL_0011, (!m_activeMoieties.empty()), "A drug model has no active moiety");
            LAMBDA_INVARIANT(
                    Invariants::INV_DRUGMODEL_0012,
                    {
                        bool ok = true;
                        for (const auto& activeMoietie : m_activeMoieties) {
                            ok &= activeMoietie->checkInvariants();
                        }
                        return ok;
                    },
                    "There is an error in an active moiety");
            INVARIANT(
                    Invariants::INV_DRUGMODEL_0013,
                    (m_timeConsiderations != nullptr),
                    "A drug model has no time consideration");
            INVARIANT(
                    Invariants::INV_DRUGMODEL_0014,
                    (m_timeConsiderations->checkInvariants()),
                    "There is an error in a drug model time consideration");
            LAMBDA_INVARIANT(
                    Invariants::INV_DRUGMODEL_0015,
                    {
                        bool ok = true;
                        for (size_t i = 0; i < m_activeMoieties.size(); i++) {
                            for (size_t j = i + 1; j < m_activeMoieties.size(); j++) {
                                // std::cout << i << ", " << j << " : " << m_activeMoieties.size() << std::endl;
                                // std::cout << m_activeMoieties[i]->getActiveMoietyId().toString().c_str() <<
                                // " , " << m_activeMoieties[j]->getActiveMoietyId().toString().c_str() << std::endl;
                                ok &=
                                        (!(m_activeMoieties[i]->getActiveMoietyId()
                                           == m_activeMoieties[j]->getActiveMoietyId()));
                            }
                        }
                        return ok;
                    },
                    "Two active moieties share the same Id");
            // No invariant on m_metadata

    )

public:
    DrugModel();

    void setDrugId(std::string _drugId)
    {
        m_drugId = std::move(_drugId);
    }
    void setDrugModelId(std::string _drugModelId)
    {
        m_drugModelId = std::move(_drugModelId);
    }

    std::string getDrugId() const
    {
        return m_drugId;
    }
    std::string getDrugModelId() const
    {
        return m_drugModelId;
    }

    void setTimeToSteadyState(Tucuxi::Common::Duration _time);

    Tucuxi::Common::Duration getTimeToSteadyState() const;

    void addCovariate(std::unique_ptr<CovariateDefinition> _covariate);

    const CovariateDefinitions& getCovariates() const;

    const FormulationAndRoutes& getFormulationAndRoutes() const;

    ParameterDefinitionIterator getParameterDefinitions(
            const AnalyteGroupId& _analyteGroupId,
            const Formulation& _formulation,
            const AdministrationRoute _route) const
    {
        ParameterDefinitionIterator iterator(*this, _analyteGroupId, _formulation, _route);
        return iterator;
    }

    ParameterDefinitionIterator getParameterDefinitions(
            const AnalyteGroupId& _analyteGroupId,
            const std::vector<const FullFormulationAndRoute*>& _formulation) const
    {
        ParameterDefinitionIterator iterator(*this, _analyteGroupId, _formulation);
        return iterator;
    }

    ParameterDefinitionIterator getParameterDefinitions(
            const AnalyteGroupId& _analyteGroupId, const std::vector<FormulationAndRoute>& _formulation) const
    {
        ParameterDefinitionIterator iterator(*this, _analyteGroupId, _formulation);
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
    const DrugModelDomain& getDomain() const
    {
        return *m_domain;
    }

    void setMetadata(std::unique_ptr<DrugModelMetadata> _metadata);
    const DrugModelMetadata& getMetadata() const
    {
        return *m_metadata;
    }

    void addAnalyteSet(std::unique_ptr<AnalyteSet> _analyteSet);

    const AnalyteSets& getAnalyteSets() const
    {
        return m_analyteSets;
    }

    void addActiveMoiety(std::unique_ptr<ActiveMoiety> _activeMoiety);

    const ActiveMoieties& getActiveMoieties() const
    {
        return m_activeMoieties;
    }

    void setTimeConsiderations(std::unique_ptr<TimeConsiderations> _timeConsiderations)
    {
        m_timeConsiderations = std::move(_timeConsiderations);
    }

    void setFormulationAndRoutes(std::unique_ptr<FormulationAndRoutes> _formulationAndRoutes)
    {
        m_formulationAndRoutes = std::move(_formulationAndRoutes);
    }

    const TimeConsiderations& getTimeConsiderations() const
    {
        return *m_timeConsiderations;
    }

    const AnalyteSet* getAnalyteSet(const AnalyteGroupId& _analyteGroupId = "") const
    {
        if ((_analyteGroupId.empty()) && (m_analyteSets.size() == 1)) {
            return m_analyteSets[0].get();
        }

        for (const std::unique_ptr<AnalyteSet>& set : m_analyteSets) {
            if (set->getId() == _analyteGroupId) {
                return set.get();
            }
        }

        // If _analyteGroupId == "" and there are multiple analytes, then we return nullptr
        // This should not happen if the function is used correctly
        return nullptr;
    }

    bool isSingleAnalyte() const
    {
        return (m_activeMoieties.size() == 1) && (m_analyteSets.size() == 1)
               && (m_analyteSets[0]->getAnalytes().size() == 1);
    }

    ///
    /// \brief Indicates if the drug model has a "dose" covariate
    /// \return true if it has a "dose" covariate, false else
    ///
    /// This function is used to know if a list of dose PatientVariate has to be generated.
    ///
    bool hasDoseCovariate() const;

    ///
    /// \brief Gets the "dose" covariate if it exists
    /// \return The CovariateDefinition of the "dose" covariate if it exists, nullptr else
    ///
    CovariateDefinition* getDoseCovariate() const;


private:
    const FullFormulationAndRoute* getFormulationAndRoute(
            const Formulation& _formulation, const AdministrationRoute _route) const
    {
        return m_formulationAndRoutes->get(_formulation, _route);
    }

    const FullFormulationAndRoute* getFormulationAndRoute(const FormulationAndRoute& _formulation) const
    {
        return m_formulationAndRoutes->get(_formulation);
    }

    const ParameterSetDefinition* getAbsorptionParameters(
            const AnalyteGroupId& _analyteGroupId,
            const Formulation& _formulation,
            const AdministrationRoute _route) const
    {
        const FullFormulationAndRoute* fr = getFormulationAndRoute(_formulation, _route);
        if (fr != nullptr) {
            return fr->getParameterDefinitions(_analyteGroupId);
        }
        return nullptr;
    }

    const ParameterSetDefinition* getAbsorptionParameters(
            const AnalyteGroupId& _analyteGroupId, const FormulationAndRoute& _formulation) const
    {
        const FullFormulationAndRoute* fr = getFormulationAndRoute(_formulation);
        if (fr != nullptr) {
            return fr->getParameterDefinitions(_analyteGroupId);
        }
        return nullptr;
    }

    const ParameterSetDefinition* getDispositionParameters(const AnalyteGroupId& _analyteGroupId) const
    {
        const AnalyteSet* pSet = getAnalyteSet(_analyteGroupId);
        if (pSet != nullptr) {
            return &pSet->getDispositionParameters();
        }
        return nullptr;
    }


    std::string m_drugId;

    std::string m_drugModelId;

    Tucuxi::Common::Duration m_timeToSteadyState;

    AnalyteSets m_analyteSets;

    std::unique_ptr<DrugModelDomain> m_domain;

    std::unique_ptr<FormulationAndRoutes> m_formulationAndRoutes;

    CovariateDefinitions m_covariates;

    InterParameterSetCorrelations m_interParameterSetCorrelations;

#ifdef DRUGMODELTESTS
public:
#endif // DRUGMODELTESTS
    ActiveMoieties m_activeMoieties;

    std::unique_ptr<TimeConsiderations> m_timeConsiderations;

    std::unique_ptr<DrugModelMetadata> m_metadata;

    friend ParameterDefinitionIterator;
    friend DrugModelChecker;
    friend TestConstantEliminationBolus;
    friend TestMultiAnalytesSingleActiveMoiety;
    friend TestMultiAnalytesMultiActiveMoieties;
    friend TestPkAsymptotic;
#ifdef DRUGMODELTESTS
    friend Drugs2Manager;
#endif // DRUGMODELTESTS
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGMODEL_H
