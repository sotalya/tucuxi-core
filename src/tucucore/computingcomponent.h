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
#include "tucucore/computingservice/computingresult.h"

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

    ///
    /// \brief compute is the entry point for any computation
    /// \param _request The request to be computed, composed of any number of single computations
    /// \param _response The response, composed of the corresponding responses to each computation
    /// \return  ComputingResult::Ok if everything went well, another value else.
    /// The response is a reference to a unique pointer that has to be allocated within compute()
    ///
    ComputingResult compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response) override;

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
    Tucuxi::Common::Interface* getInterface(const std::string &_name) override;


private:

    std::unique_ptr<ComputingUtils> m_utils;

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

    ComputingResult computePercentilesSimple(
            const ComputingTraitPercentiles *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);

    ComputingResult computePercentilesMulti(
            const ComputingTraitPercentiles *_traits,
            const ComputingRequest &_request,
            std::unique_ptr<ComputingResponse> &_response);


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
