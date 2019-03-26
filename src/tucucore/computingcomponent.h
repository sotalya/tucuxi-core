#ifndef TUCUXI_CORE_COMPUTINGCOMPONENT_H
#define TUCUXI_CORE_COMPUTINGCOMPONENT_H


#include <memory>

#include "tucucommon/component.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/definitions.h"
#include "tucucore/residualerrormodel.h"
#include "tucucore/dosage.h"
#include "tucucore/sampleevent.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/concentrationprediction.h"
#include "tucucore/idatamodelservices.h"
#include "tucucore/pkmodel.h"
#include "tucucore/covariateevent.h"
#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingtrait.h"

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



///
/// \brief The main entry point for any computation.
/// It offers a method that takes as input a computing request, and that outputs a computing response.
///
class ComputingComponent : public Tucuxi::Common::Component,
        public IComputingService
{
public:

    static Tucuxi::Common::Interface* createComponent();

    /// \brief Destructor
    ~ComputingComponent() override;

    ComputingResult compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response) override;
    std::string getErrorString() const override;

    void setPkModelCollection(PkModelCollection *_collection) {
        if (m_models != nullptr) {
            delete m_models;
        }
        m_models = _collection;
    }

protected:
    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string &_name) override;


private:

    PkModelCollection *m_models;
    GeneralExtractor *m_generalExtractor;

    Tucuxi::Common::LoggerHelper m_logger;

    /// \brief Constructor called from createComponent()
    ComputingComponent();

    bool initialize();


    ComputingResult compute(
            const ComputingTraitConcentration *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputingResult compute(
            const ComputingTraitPercentiles *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputingResult compute(
            const ComputingTraitAdjustment *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputingResult compute(
            const ComputingTraitAtMeasures *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputingResult compute(
            const ComputingTraitSinglePoints *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputationResult computeConcentrations(
        ConcentrationPredictionPtr &_prediction,
        bool _isAll,
        const DateTime &_recordFrom,
        const DateTime &_recordTo,
        const IntakeSeries &_intakes,
        const ParameterSetSeries& _parameters,
        const Etas& _etas = Etas(0),
        const IResidualErrorModel &_residualErrorModel = EMPTY_RESIDUAL_ERROR_MODEL,
        const Deviations& _eps = Deviations(0),
        bool _isFixedDensity = 0);

    std::vector<FullDosage> sortAndFilterCandidates(std::vector<FullDosage> &_candidates, BestCandidatesOption _option);

    typedef struct {
        Value m_dose;
        Duration m_interval;
        Duration m_infusionTime;
    } AdjustmentCandidate;


    ComputingResult buildCandidates(const FullFormulationAndRoute* _formulationAndRoute, std::vector<AdjustmentCandidate> &_candidates);

    DosageTimeRange *createDosage(
            ComputingComponent::AdjustmentCandidate &_candidate,
            DateTime _startTime,
            DateTime _endTime,
            FormulationAndRoute _routeOfAdministration);

    friend class ComputingTraitSinglePoints;
    friend class ComputingTraitAtMeasures;
    friend class ComputingTraitAdjustment;
    friend class ComputingTraitConcentration;
    friend class ComputingTraitPercentiles;


};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_COMPUTINGCOMPONENT_H
