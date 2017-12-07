/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_PERCENTILESPREDICTION_H
#define TUCUXI_CORE_PERCENTILESPREDICTION_H

#include <fstream>
#include <memory>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class PercentilesPrediction
{
public:
    bool streamToFile(const std::string _fileName) 
    {
        std::ofstream ostrm(_fileName, std::ios::binary);
        if (ostrm.rdstate() & std::ios_base::failbit)
            return false;

        size_t nbCycles = this->m_times.size();
        double offset = 0.0;
        for (size_t cycle = 0; cycle < nbCycles; cycle++) {
            const Tucuxi::Core::TimeOffsets times = m_times[cycle];
            size_t nbPoints = times.size();
            for (size_t i = 0; i < nbPoints - 1; i++) {
                ostrm << (times[i]) + offset << " ";
                for (size_t perc = 0; perc < m_values.size(); perc++) {
                    ostrm << m_values[perc][cycle][i] << " ";
                }
                ostrm << std::endl;
            }
            offset += times[nbPoints - 1];
        }
        return true;
    }

    void init(const PercentileRanks& _ranks, const std::vector<TimeOffsets> &_times, size_t _nCycles, int _nPoints)
    {
        m_ranks = _ranks;
        m_times = _times;
        m_values.clear();

        PercentileRanks::iterator itRank = m_ranks.begin();
        while (itRank != m_ranks.end()) {
            std::vector< std::vector<Concentration> > vec;
            for (size_t cycle = 0; cycle < _nCycles; cycle++) {
                vec.push_back(std::vector<Concentration>(_nPoints));
            }
            m_values.push_back(vec);
            itRank++;
        }        
    }

    const PercentileRanks& getRanks() const { return m_ranks; }
    const std::vector<TimeOffsets>& getTimes() const { return m_times; }
    const std::vector<std::vector<std::vector<Value> > >& getValues() const { return m_values; }

    void appendPercentile(int _percentileRank, int _cycle, int _point, Value _value) 
    { 
        m_values[_percentileRank][_cycle][_point] = _value; 
    }

private:
    std::vector<TimeOffsets> m_times;
    PercentileRanks m_ranks;
    std::vector<std::vector<std::vector<Value> > >m_values; // Cylces->Points->Value
};
typedef std::unique_ptr<PercentilesPrediction> PercentilesPredictionPtr;

}
}

#endif // TUCUXI_CORE_PERCENTILESPREDICTION_H