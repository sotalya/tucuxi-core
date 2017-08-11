/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_IPROCESSINGSERVICES_H
#define TUCUXI_CORE_IPROCESSINGSERVICES_H

#include <memory>
#include <vector>

#include "tucucommon/interface.h"
#include "tucucommon/datetime.h"

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

enum class PredictionType { Population, Apiori, Aposteriori };

class ConcentrationPrediction
{
public:
    bool allocate(const CycleSize _nbPoints, TimeOffsets &_times, Concentrations &_values)
    {
        _times.reserve(_nbPoints);
        _values.reserve(_nbPoints);
        return true;
    }
    void appendConcentrations(TimeOffsets &_times, Concentrations &_values)
    {
        m_times.push_back(_times);
        m_values.push_back(_values);
    }

private:
    std::vector<TimeOffsets> m_times;
    std::vector<Concentrations> m_values;
};
typedef std::unique_ptr<ConcentrationPrediction> ConcentrationPredictionPtr;

typedef int PercentileRank;
typedef std::vector<PercentileRank> PercentileRanks;
typedef Value Percentile;
typedef std::vector<Percentile> Percentiles;

class PercentilesPrediction
{
private:
    std::vector<int> m_times;
    PercentileRanks m_ranks;
    std::vector<Percentiles> m_values;
};
typedef std::unique_ptr<PercentilesPrediction> PercentilesPredictionPtr;

class IProcessingServices : public Tucuxi::Common::Interface
{
public:
    virtual ConcentrationPredictionPtr computeConcentrations(PredictionType _type, 
                                                             Tucuxi::Common::DateTime _start, 
                                                             Tucuxi::Common::DateTime _end, 
                                                             int _nbPoints) = 0;
    virtual PercentilesPredictionPtr computePercentiles(PredictionType _type, 
                                                        Tucuxi::Common::DateTime _start, 
                                                        Tucuxi::Common::DateTime _end, 
                                                        const PercentileRanks &_ranks, 
                                                        int _nbPoints) = 0;
    virtual void computeAdjustments() = 0;
};

}
}

#endif // TUCUXI_CORE_IPROCESSINGSERVICES_H