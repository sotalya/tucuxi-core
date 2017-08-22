/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CORECOMPONENT_H
#define TUCUXI_CORE_CORECOMPONENT_H

#include <memory>

#include "tucucommon/component.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/definitions.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/dosage.h"
#include "tucucore/sample.h"
#include "tucucore/iprocessingservices.h"
#include "tucucore/idatamodelservices.h"

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugErrorModel;
class DrugTreatment;
class ParameterSetSeries;

enum class ComputationResult { Success, Failure, Aborted };

class CoreComponent : public Tucuxi::Common::Component, 
                      public IProcessingServices,
                      public IDataModelServices
{
public:
    /// \brief Constructor call by LoggerHelper
    ~CoreComponent();

    bool loadDrug(const std::string& _xmlDrugDescription) override;
    bool loadTreatment(const std::string& _xmlTreatmentDescription) override;
        
    virtual ConcentrationPredictionPtr computeConcentrations(const ConcentrationRequest &_request) override;
    virtual PercentilesPredictionPtr computePercentiles(const PercentilesRequest &_request) override;

    void computeAdjustments() override;

protected:
    /// \brief Access other interfaces of the same component.
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name);

private:
    /// \brief Constructor call by LoggerHelper
    CoreComponent(const std::string &_filename);

    ComputationResult computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas = Etas(0),
        const ResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
        const Deviation& _eps = 0,
        const bool _isFixedDensity = 0);

    ComputationResult computePopulation(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters)
    {
        return computeConcentrations(_prediction, _nbPoints, _intakes, _parameters);
    }

    ComputationResult computeApriori(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters)
    {
        return computeConcentrations(_prediction, _nbPoints, _intakes, _parameters);
    }

    ComputationResult computeAposteriori(
        ConcentrationPredictionPtr &_prediction,
        int _nbPoints,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas)
    {
        return computeConcentrations(_prediction, _nbPoints, _intakes, _parameters, _etas);
    }

    ComputationResult extractError(
        const DrugErrorModel &_errorMode,
        const ParameterDefinitions &_parameterDefs,
        OmegaMatrix &_omega,
        ResidualErrorModel &residualErrorModel)
    {
        // TODO YJE
        return ComputationResult::Failure;
    }

private:
    Tucuxi::Common::LoggerHelper m_logger;
    std::unique_ptr<DrugModel> m_drug;
    std::unique_ptr<DrugTreatment> m_treatment;
};

}
}

#endif // TUCUXI_CORE_CORECOMPONENT_H
