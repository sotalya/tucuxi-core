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


#ifndef CACHECOMPUTING_H
#define CACHECOMPUTING_H

#include <memory>
#include <mutex>
#include <vector>

#include "tucucore/computingservice/computingresponse.h"
#include "tucucore/computingservice/computingresult.h"

namespace Tucuxi {
namespace Core {

class IComputingService;
class ComputingRequest;
class ComputingResponse;
class ComputedData;

class CacheComputing
{
public:
    CacheComputing(IComputingService* _computingComponent);

    ///
    /// \brief computes
    /// \param _request The request containing all information required for the computation
    /// \param _response The result of the computation
    /// \return ComputingResult::Ok if everything went well with the computation, another code else
    /// The response is a reference to a unique pointer that has to be allocated within compute()
    ///
    /// Currently, only percentiles calculations are handled by the cache.
    ///
    ComputingStatus compute(const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response);

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
    bool getFromCache(const ComputingRequest& _request, std::unique_ptr<ComputingResponse>& _response);

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
    bool getSpecificIntervalFromCache(
            const DateTime& _start,
            const DateTime& _end,
            double _nbPointsPerHour,
            std::unique_ptr<ComputingResponse>& _response);

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
    bool buildResponse(
            const DateTime& _start,
            const DateTime& _end,
            double _nbPointsPerHour,
            const std::vector<PercentilesData*>& _candidates,
            std::unique_ptr<ComputingResponse>& _response);

    /// A mutex to protect the compute() method
    std::mutex m_mutex;

    /// The computing component used by the cache, initialized within the constructor
    IComputingService* m_computingComponent;

    /// A vector containing all data previously calculated
    std::vector<std::unique_ptr<ComputedData> > m_data;

    /// Indicates if the last call to compute() was a hit (true) or a miss (false)
    bool m_isLastCallaHit{false};

    /// This structure embeds information about a cycle data added in m_indexVector
    typedef struct
    {
        PercentilesData* m_set;   ///! The percentile data containing the cycle data
        std::size_t m_cycleIndex; ///! Index of the cycle data
        DateTime m_start;         ///! Start date of the cycle data
        DateTime m_end;           ///! End date of the cycle data
    } index_t;

    /// A vector meant to embed information about the cycle datas that fill
    /// the interval of interest. If the cache contain enough data, then
    /// the content of this vector, after buildIndex() is called, should
    /// fill the interval, each item having a starting date equal to the ending
    /// date of the previous index.
    std::vector<index_t> m_indexVector;

    ///
    /// \brief Builds an index of cycle data to fill the interval
    /// \param _start Start date of the cycle data
    /// \param _end End date of the cycle data
    /// \param _nbPointsPerHour Minimum numbre of points per hours that should be retrieved
    /// \param _candidates List of PercentilesData candidates
    ///
    /// This function assumes each candidate has an overlap with the [_start, _end] interval.
    /// It fills m_indexVector with indexes to cycle datas
    ///
    void buildIndex(
            const DateTime& _start,
            const DateTime& _end,
            double _nbPointsPerHour,
            const std::vector<PercentilesData*>& _candidates);

    ///
    /// \brief Tries to insert a cycle data into the m_indexVector
    /// \param _start Start date of the cycle data
    /// \param _end End date of the cycle data
    /// \param _data The original percentiles data
    /// \param cycleIndex The cycle of interest in the original percentiles data
    ///
    /// This function iterates within the m_indexVector to find an empty slot corresponding to the
    /// [_start,_end] interval. If it finds one, it inserts an index_t at this specific slot.
    /// If the slot is already filled, then nothing happens.
    ///
    void insertCycle(const DateTime& _start, DateTime _end, PercentilesData* _data, std::size_t _cycleIndex);

    ///
    /// \brief Checks if the data referenced by m_indexVector fills the interval
    /// \param _start Start date of the interval
    /// \param _end End date of the interval
    /// \return true if m_indexVector contains data for the entire interval, false else
    ///
    /// This function shall be called after buildIndex(), to check if the built index fills the
    /// [_start, _end] interval
    ///
    bool isFullIntervalInCache(const DateTime& _start, const DateTime& _end);
};

} // namespace Core
} // namespace Tucuxi

#endif // CACHECOMPUTING_H
