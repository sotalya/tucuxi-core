/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CORECOMPONENT_H
#define TUCUXI_CORE_CORECOMPONENT_H

#include <memory>

#include "tucucommon/component.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/dosage.h"
#include "tucucore/sampleevent.h"
#include "tucucore/processingservice/iprocessingservice.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/idatamodelservices.h"

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugErrorModel;
class DrugTreatment;
class ParameterSetSeries;

class CoreComponent : public Tucuxi::Common::Component,
                      public IProcessingService,
                      public IDataModelServices
{
public:
    static Tucuxi::Common::Interface* createComponent();

    /// \brief Destructor
    ~CoreComponent();

    bool loadDrug(const std::string& _xmlDrugDescription) override;
    bool loadTreatment(const std::string& _xmlTreatmentDescription) override;
    
    ProcessingResult compute(const ProcessingRequest &_request, std::unique_ptr<ProcessingResponse> &_response);
    std::string getErrorString() const;

protected:
    /// \brief Access other interfaces of the same component.
    virtual Tucuxi::Common::Interface* getInterface(const std::string &_name) override;
    

private:
    /// \brief Constructor called from createComponent()
    CoreComponent();

    ComputationResult computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        const bool _isAll,
        int _nbPoints,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas = Etas(0),
        const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
        const Deviations& _eps = Deviations(0),
        const bool _isFixedDensity = 0);

    ComputationResult computePopulation(
        ConcentrationPredictionPtr &_prediction,
        const bool _isAll,
        int _nbPoints,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters)
    {
        return computeConcentrations(_prediction, _isAll, _nbPoints, _recordFrom, _recordTo, _intakes, _parameters);
    }

    ComputationResult computeApriori(
        ConcentrationPredictionPtr &_prediction,
        const bool _isAll,
        int _nbPoints,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters)
    {
        return computeConcentrations(_prediction, _isAll, _nbPoints, _recordFrom, _recordTo, _intakes, _parameters);
    }

    ComputationResult computeAposteriori(
        ConcentrationPredictionPtr &_prediction,
        const bool _isAll,
        int _nbPoints,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas)
    {
        TMP_UNUSED_PARAMETER(_etas);
        return computeConcentrations(_prediction, _isAll, _nbPoints, _recordFrom, _recordTo, _intakes, _parameters);
    }

    ComputationResult extractError(
        const DrugErrorModel &_errorMode,
        const ParameterDefinitions &_parameterDefs,
        OmegaMatrix &_omega,
        IResidualErrorModel &_residualErrorModel)
    {
        TMP_UNUSED_PARAMETER(_errorMode);
        TMP_UNUSED_PARAMETER(_parameterDefs);
        TMP_UNUSED_PARAMETER(_omega);
        TMP_UNUSED_PARAMETER(_residualErrorModel);
        // TODO YJE
        return ComputationResult::Failure;
    }

private:
    Tucuxi::Common::LoggerHelper m_logger;
    std::unique_ptr<DrugModel> m_drug;
    std::unique_ptr<DrugTreatment> m_treatment;

    friend class ProcessingTraitSinglePoints;
    friend class ProcessingTraitAtMeasures;
    friend class ProcessingTraitAdjustment;
    friend class ProcessingTraitConcentration;
    friend class ProcessingTraitPercentiles;
};

}
}

#endif // TUCUXI_CORE_CORECOMPONENT_H

