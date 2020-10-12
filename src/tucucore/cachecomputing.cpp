#include "cachecomputing.h"

#include "computingservice/icomputingservice.h"
#include "computingservice/computingrequest.h"
#include "computingservice/computingresponse.h"

namespace Tucuxi {
namespace Core {

CacheComputing::CacheComputing(IComputingService *_computingComponent) : m_computingComponent(_computingComponent)
{

}

ComputingStatus CacheComputing::compute(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response)
{
    // Ensure the function is reentrant
    std::lock_guard<std::mutex> lock(m_mutex);

    if (getFromCache(_request, _response)) {
        m_isLastCallaHit = true;
//        std::cout << "Using the cache" << std::endl;
        return ComputingStatus::Ok;
    }

    m_isLastCallaHit = false;

    auto response = std::make_unique<ComputingResponse>(_response->getId());

    auto result = m_computingComponent->compute(_request, _response);

    const auto d = dynamic_cast<const PercentilesData*>(_response->getData());

    if (d != nullptr) {
        // This call creates a copy of the percentiles data. It works because it operates
        // a deep copy.
        m_data.emplace_back(std::make_unique<PercentilesData>(*d));
    }

    return result;
}

void CacheComputing::clear()
{
//    std::cout << "Clearing the cache" << std::endl;
    m_data.clear();
}

bool CacheComputing::isLastCallaHit() const
{
    return m_isLastCallaHit;
}

bool CacheComputing::getSpecificIntervalFromCache(DateTime _start, DateTime _end, double _nbPointsPerHour, std::unique_ptr<ComputingResponse> &_response)
{
    // This function implements various ways of getting pertinent data from the Cache.

    std::vector<PercentilesData*> candidates;

    for (const auto &data : m_data) {
        PercentilesData *pData = dynamic_cast<PercentilesData*>(data.get());
        if (pData != nullptr) {
            // We have cycles of the first percentiles through pData->getPercentileData(0)
            const auto firstCycle = pData->getPercentileData(0).front();
            const auto lastCycle = pData->getPercentileData(0).back();
            // We need at least the number of points required by the caller
            if (pData->getNbPointsPerHour() >= _nbPointsPerHour) {

                // Is the cache data being a superset of the interval asked?
                // If yes, then just return that data
                if ((firstCycle.m_start <= _start) && (lastCycle.m_end >= _end)) {
                    _response->addResponse(std::make_unique<PercentilesData>(*pData));
                    return true;
                }

                // Is there an overlap between the cached data and the interval?
                // If yes, then keep a reference on the candidate
                if (((firstCycle.m_start <= _start) && (lastCycle.m_end > _start))
                || ((firstCycle.m_start < _end) && (lastCycle.m_end >= _end))) {
                    candidates.push_back(pData);
                }
            }
        }
    }
    // Try to build a response based on the overlapping candidates
    return buildResponse(_start, _end, _nbPointsPerHour, candidates, _response);
}


bool CacheComputing::buildResponse(DateTime _start, DateTime _end, double _nbPointsPerHour, std::vector<PercentilesData*> _candidates, std::unique_ptr<ComputingResponse> &_response)
{
    return false;
}

bool CacheComputing::getFromCache(const ComputingRequest &_request, std::unique_ptr<ComputingResponse> &_response)
{
    // We iterate over the computing traits, and only process the percentiles,
    // by calling getSpecificIntervalFromCache
    for (const auto & trait : _request.getComputingTraits()) {
        ComputingTraitPercentiles *pTraits = dynamic_cast<ComputingTraitPercentiles*>(trait.get());
        if (pTraits != nullptr) {
            auto start = pTraits->getStart();
            auto end = pTraits->getEnd();
            auto nbPointsPerHour = pTraits->getNbPointsPerHour();
            if (getSpecificIntervalFromCache(start, end, nbPointsPerHour, _response)) {
                return true;
            }
        }
    }
    return false;
}

std::string CacheComputing::getErrorString() const
{
    return m_computingComponent->getErrorString();
}

} // namespace Core
} // namespace Tucuxi


