//@@license@@

#ifndef TUCUXI_CORE_COMPUTINGCOMPONENT_H
#define TUCUXI_CORE_COMPUTINGCOMPONENT_H


#include <memory>

#include "tucucommon/component.h"
#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/covariateevent.h"
#include "tucucore/definitions.h"
#include "tucucore/dosage.h"
#include "tucucore/idatamodelservices.h"
#include "tucucore/pkmodel.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/sampleevent.h"

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugErrorModel;
class DrugTreatment;
class PkModelCollection;
class ParameterSetSeries;
class ComputingTraitPercentiles;
class ComputingTraitConcentration;
class ComputingTraitAdjustment;
class ComputingTraitAtMeasures;
class ComputingTraitSinglePoints;
class ComputingTraitStandard;
class HalfLife;
class GeneralExtractor;
class ActiveMoiety;
class ComputingAdjustments;
class ComputingUtils;
class PercentilesPrediction;


///
/// \brief The main entry point for any computation.
/// It offers a method that takes as input a computing request, and that outputs a computing response.
///
class ComputingComponent : public Tucuxi::Common::Component, public IComputingService
{
public:
    static Tucuxi::Common::Interface* createComponent();

    /// \brief Destructor
    ~ComputingComponent() override = default;

    ///
    /// \brief compute is the entry point for any computation
    /// \param _request The request to be computed, composed of any number of single computations
    /// \param _response The response, composed of the corresponding responses to each computation
    /// \return  ComputingResult::Ok if everything went well, another value else.
    /// The response is a reference to a unique pointer that has to be allocated within compute()
    ///
    ComputingStatus compute(const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response) override;

    ///
    /// \brief returns a description of the last error in case of failed computation
    /// \return A description of the last error
    /// This function should only be called if compute() returned something different from ComputingResult::Ok
    /// This function is not yet implemented
    std::string getErrorString() const override;

    ///
    /// \brief setPkModelCollection sets the Pk models collection to be used in further computations
    /// \param _collection A shared pointer to the Pk models collection
    /// By default, at creation, the ComputingComponent calls defaultPopulate() to create the Pk
    /// models collection. This function allows to override it with a custom collection.
    ///
    void setPkModelCollection(std::shared_ptr<PkModelCollection> _collection);

protected:
    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override;


private:
    std::unique_ptr<ComputingUtils> m_utils;

    Tucuxi::Common::LoggerHelper m_logger;

    /// \brief Constructor called from createComponent()
    ComputingComponent();

    bool initialize();


    ComputingStatus compute(
            const ComputingTraitConcentration* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus compute(
            const ComputingTraitPercentiles* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus compute(
            const ComputingTraitAdjustment* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus compute(
            const ComputingTraitAtMeasures* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus compute(
            const ComputingTraitSinglePoints* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus computePercentilesSimple(
            const ComputingTraitPercentiles* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus computePercentilesMulti(
            const ComputingTraitPercentiles* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response);

    ComputingStatus preparePercentilesResponse(
            const ComputingTraitPercentiles* _traits,
            const ComputingRequest& _request,
            std::unique_ptr<ComputingResponse>& _response,
            GroupsIntakeSeries& _intakeSeries,
            const ConcentrationPredictionPtr& _pPrediction,
            const PercentilesPrediction& _percentiles,
            const PercentileRanks& _percentileRanks);

    static ComputingStatus recordCycle(
            const ComputingTraitStandard* _traits,
            const ComputingRequest& _request,
            ConcentrationData& _concentrationData,
            DateTime _start,
            DateTime _end,
            const TimeOffsets& _times,
            const std::vector<ConcentrationPredictionPtr>& _activeMoietiesPredictions,
            const std::vector<ConcentrationPredictionPtr>& _analytesPredictions,
            size_t _valueIndex,
            const std::map<AnalyteGroupId, Etas>& _etas,
            GroupsParameterSetSeries& _parameterSeries);

    static void endRecord(
            const ComputingTraitStandard* _traits,
            const ComputingRequest& _request,
            ConcentrationData& _concentrationData);


    friend class ComputingTraitSinglePoints;
    friend class ComputingTraitAtMeasures;
    friend class ComputingTraitAdjustment;
    friend class ComputingTraitConcentration;
    friend class ComputingTraitPercentiles;
    friend class ComputingAdjustments;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGCOMPONENT_H
