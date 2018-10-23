/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_CONCENTRATIONPREDICTION_H
#define TUCUXI_CORE_CONCENTRATIONPREDICTION_H

#include <fstream>
#include <memory>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

class ConcentrationPrediction
{
public:

    bool streamToFile(const std::string _fileName) 
    {
        std::ofstream ostrm(_fileName, std::ios::binary);
        if (ostrm.rdstate() & std::ios_base::failbit) {
            return false;
        }

        size_t nbCycles = this->m_values.size();
        double offset = 0.0;
        for (size_t cycle = 0; cycle < nbCycles; cycle++) {
            const Tucuxi::Core::Concentrations concentrations = getValues()[cycle];
            const Tucuxi::Core::TimeOffsets times = getTimes()[cycle];
            size_t nbPoints = concentrations.size();
            for (size_t i = 0; i < nbPoints - 1; i++) {
                ostrm << (times[i]) + offset << " " << concentrations[i] << std::endl;
            }
            offset += times[nbPoints - 1];
        }
        return true;
    }

    bool allocate(const unsigned int _residualSize, const CycleSize _nbPoints, TimeOffsets &_times, std::vector<Concentrations> &_values) const
    {
        _times.reserve(_nbPoints);
        for (unsigned int compartement = 0; compartement < _residualSize; compartement++) {
            _values[compartement].reserve(_nbPoints);
        }
        return true;
    }

    void appendConcentrations(TimeOffsets &_times, Concentrations &_values)
    {
        m_times.push_back(_times);
        m_values.push_back(_values);
    }

    std::vector<TimeOffsets> getTimes() const { return m_times; }
    std::vector<Concentrations> getValues() const { return m_values; }

private:
    std::vector<TimeOffsets> m_times;
    std::vector<Concentrations> m_values;
};
typedef std::unique_ptr<ConcentrationPrediction> ConcentrationPredictionPtr;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_CONCENTRATIONPREDICTION_H

