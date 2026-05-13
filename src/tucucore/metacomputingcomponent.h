//@@license@@

#ifndef TUCUXI_CORE_METACOMPUTINGCOMPONENT_H
#define TUCUXI_CORE_METACOMPUTINGCOMPONENT_H

#include "tucucommon/component.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/icomputingservice.h"
#include "tucucore/pkmodel.h"


namespace Tucuxi {
namespace Core {

class ComputingComponent;
class MultiComputingComponent;
class IComputingService;

///
/// \brief The main entry point for any computation.
/// It offers a method that takes as input a computing request, and that outputs a computing response.
///
class MetaComputingComponent : public Tucuxi::Common::Component, public IComputingService
{
public:
    static Tucuxi::Common::Interface* createComponent();

    /// \brief Destructor
    ~MetaComputingComponent() override = default;

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
    void setPkModelCollection(std::shared_ptr<PkModelCollection> _collection) override;

protected:
    /// \brief Access other interfaces of the same component.
    Tucuxi::Common::Interface* getInterface(const std::string& _name) override;


private:
    /// \brief Constructor called from createComponent()
    MetaComputingComponent();

    bool initialize();

    std::unique_ptr<IComputingService> m_computingComponent;
    std::unique_ptr<IComputingService> m_multiComputingComponent;
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_METACOMPUTINGCOMPONENT_H
