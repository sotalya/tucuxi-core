#ifndef CACHECOMPUTING_H
#define CACHECOMPUTING_H

#include <memory>
#include <mutex>
#include <vector>

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/computingservice/computingresponse.h"

namespace Tucuxi {
namespace Core {

class IComputingService;
class ComputingRequest;
class ComputingResponse;
class ComputedData;

class CacheComputing
{
public:
    CacheComputing(IComputingService *_computingComponent);

    ///
    /// \brief computes
    /// \param _request The request containing all information required for the computation
    /// \param _response The result of the computation
    /// \return ComputingResult::Ok if everything went well with the computation, another code else
    /// The response is a reference to a unique pointer that has to be allocated within compute()
    ///
    /// Currently, only percentiles calculations are handled by the cache.
    ///
    ComputingStatus compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response);

    ///
    /// \brief returns a description of the last error in case of failed computation
    /// \return A description of the last error
    /// This function should only be called if compute() returned something different from ComputingResult::Ok
    ///
    std::string getErrorString() const;

    ///
    /// \brief Clears the cache data
    ///
    void clear();

    ///
    /// \brief Indicates if the last computation was a hit
    /// \return true if the last call to compute was found in the cache, false else
    ///
    bool isLastCallaHit() const;

protected:

    ///
    /// \brief Tries to get data from the cache
    /// \param _request The computing request to calculate
    /// \param _response The computing response that has to be filled with the cached data
    /// \return true if data was found in the cache, false else
    ///
    /// If data is found in the cache, the _response is filled when the function returns.
    /// If data is not found in the cache, then the _response is unchanged.
    ///
    bool getFromCache(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response);

    ///
    /// \brief Tries to get data for a specific interval from the cache
    /// \param _start Start date of the interval
    /// \param _end End date of the interval
    /// \param _nbPointsPerHour Minimum numbre of points per hours that should be retrieved
    /// \param _response The computing response that has to be filled with the cached data
    /// \return true if data was found in the cache, false else
    ///
    /// If data is found in the cache, the _response is filled when the function returns.
    /// If data is not found in the cache, then the _response is unchanged.
    ///
    bool getSpecificIntervalFromCache(DateTime _start, DateTime _end, double _nbPointsPerHour, std::unique_ptr<ComputingResponse> &_response);

    ///
    /// \brief buildResponse
    /// \param _start Start date of the interval
    /// \param _end End date of the interval
    /// \param _nbPointsPerHour Minimum numbre of points per hours that should be retrieved
    /// \param _candidates A vector of candidate intervals already extracted
    /// \param _response The computing response that has to be filled with the cached data
    /// \return true if data was found in the cache, false else
    ///
    /// If data is found in the cache, the _response is filled when the function returns.
    /// If data is not found in the cache, then the _response is unchanged.
    ///
    /// This function assumes the candidates have an overlad with the [_start, _end] one, and have
    /// at least the required number of points per hour.
    /// It tries to build a response set from the candidates, and populate the response if possible.
    ///
    bool buildResponse(DateTime _start, DateTime _end, double _nbPointsPerHour, std::vector<PercentilesData*> _candidates, std::unique_ptr<ComputingResponse> &_response);

    /// A mutex to protect the compute() method
    std::mutex m_mutex;

    /// The computing component used by the cache, initialized within the constructor
    IComputingService *m_computingComponent;

    /// A vector containing all data previously calculated
    std::vector<std::unique_ptr<ComputedData> > m_data;

    /// Indicates if the last call to compute() was a hit (true) or a miss (false)
    bool m_isLastCallaHit{false};

};

} // namespace Core
} // namespace Tucuxi

#endif // CACHECOMPUTING_H
