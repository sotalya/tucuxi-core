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


#ifndef TUCUXI_CORE_PERCENTILESPREDICTION_H
#define TUCUXI_CORE_PERCENTILESPREDICTION_H

#include <fstream>
#include <memory>

#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

class PercentilesPrediction
{
public:
    bool streamToFile(const std::string& _fileName)
    {
        std::ofstream ostrm(_fileName, std::ios::binary);
        if ((ostrm.rdstate() & std::ios_base::failbit) != 0) {
            return false;
        }

        size_t nbCycles = this->m_times.size();
        double offset = 0.0;
        for (size_t cycle = 0; cycle < nbCycles; cycle++) {
            const Tucuxi::Core::TimeOffsets times = m_times[cycle];
            size_t nbPoints = times.size();
            for (size_t i = 0; i < nbPoints - 1; i++) {
                ostrm << (times[i]) + offset << " ";
                for (auto& value : m_values) {
                    ostrm << value[cycle][i] << " ";
                }
                ostrm << std::endl;
            }
            offset += times[nbPoints - 1];
        }
        return true;
    }

    void init(const PercentileRanks& _ranks, const std::vector<TimeOffsets>& _times, const IntakeSeries& _intakes)
    {
        m_ranks = _ranks;
        m_times = _times;
        m_values.clear();

        auto itRank = m_ranks.begin();
        while (itRank != m_ranks.end()) {
            std::vector<Concentrations> vec;
            for (const auto& intake : _intakes) {
                vec.emplace_back(Concentrations(intake.getNbPoints()));
            }
            m_values.push_back(vec);
            itRank++;
        }
    }

    const PercentileRanks& getRanks() const
    {
        return m_ranks;
    }
    const std::vector<TimeOffsets>& getTimes() const
    {
        return m_times;
    }
    const std::vector<std::vector<Concentrations> >& getValues() const
    {
        return m_values;
    }

    void appendPercentile(size_t _percentileRank, size_t _cycle, size_t _point, Value _value)
    {
        m_values[_percentileRank][_cycle][_point] = _value;
    }

private:
    std::vector<TimeOffsets> m_times;
    PercentileRanks m_ranks;
    std::vector<std::vector<Concentrations> > m_values; // Cycles->Points->Value
};
typedef std::unique_ptr<PercentilesPrediction> PercentilesPredictionPtr;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_PERCENTILESPREDICTION_H
