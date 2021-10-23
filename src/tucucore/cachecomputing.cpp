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

bool CacheComputing::getSpecificIntervalFromCache(
        DateTime _start,
        DateTime _end,
        double _nbPointsPerHour,
        std::unique_ptr<ComputingResponse> &_response)
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


bool CacheComputing::buildResponse(
        DateTime _start,
        DateTime _end,
        double _nbPointsPerHour,
        const std::vector<PercentilesData*> &_candidates,
        std::unique_ptr<ComputingResponse> &_response)
{
    buildIndex(_start, _end, _nbPointsPerHour, _candidates);
    if (isFullIntervalInCache(_start, _end)) {
        PercentilesData pData(_response->getId());
        for (const auto &index : m_indexVector) {
            if (index.m_end > _start) {
                const auto &ranks = _candidates[0]->getRanks();
                pData.setRanks(ranks);
                pData.setNbPointsPerHour(_nbPointsPerHour);
                for (size_t rankIndex = 0; rankIndex < index.m_set->getNbRanks(); rankIndex ++) {
                    pData.addPercentileData(index.m_set->getPercentileData(rankIndex));
                }
            }
        }
        _response->addResponse(std::make_unique<PercentilesData>(pData));
        return true;
    }
    return false;
}

bool CacheComputing::isFullIntervalInCache(DateTime _start, DateTime _end)
{
    if (m_indexVector.empty()) {
        return false;
    }
    if (m_indexVector[0].m_start > _start) {
        return false;
    }
    if (m_indexVector.back().m_end < _end) {
        return false;
    }
    DateTime lastEnd = m_indexVector.front().m_start;
    for (const auto & index : m_indexVector) {
        if (index.m_start != lastEnd) {
            return false;
        }
        lastEnd = index.m_end;
    }
    return true;
}

void CacheComputing::buildIndex(
        DateTime _start,
        DateTime _end,
        double _nbPointsPerHour,
        const std::vector<PercentilesData*> &_candidates)
{
    TMP_UNUSED_PARAMETER(_nbPointsPerHour);
    m_indexVector.clear();
    for (const auto &data : _candidates) {
        PercentilesData *pData = dynamic_cast<PercentilesData*>(data);
        if (pData != nullptr) {
            const auto p0 = pData->getPercentileData(0);
            const auto size = p0.size();
            for (size_t i = 0; i < size; i++) {
                auto start = p0[i].m_start;
                auto end = p0[i].m_end;
                if ((end > _start) && (start < _end)) {
                    insertCycle(start, end, pData, i);
                }
            }
        }
    }
}

void CacheComputing::insertCycle(DateTime _start, DateTime _end, PercentilesData* _data, std::size_t _cycleIndex)
{
    auto it = m_indexVector.rbegin();
    while (it != m_indexVector.rend()) {
        if (_start == (*it).m_start) {
            return;
        }
        if (_start > (*it).m_start) {
            break;
        }
        it ++;
    }
    m_indexVector.insert(it.base(), index_t{_data, _cycleIndex, _start, _end});
/*
    size_t insertIndex = m_indexVector.size();
    for(size_t i = m_indexVector.size() - 1; i >= 0; i--) {
        if (_start == m_indexVector[i].m_start) {
            return;
        }
        if (_start > m_indexVector[i].m_start) {
            break;
        }
        insertIndex --;
    }

    auto vit = m_indexVector.begin() + insertIndex;

    m_indexVector.insert(vit, index_t{_data, cycleIndex, _start, _end});
    */
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


