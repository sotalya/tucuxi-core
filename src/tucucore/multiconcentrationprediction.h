/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_MULTICONCENTRATIONPREDICTION_H
#define TUCUXI_CORE_MULTICONCENTRATIONPREDICTION_H

#include <fstream>
#include <memory>

#include "tucucore/definitions.h"

namespace Tucuxi {
namespace Core {

///
/// \brief The MultiConcentrationPrediction class
///
/// This class embeds results of a MultiConcentrationCalculator computation.
/// It owns time values and concentration values, organized as:
///
/// A vector of intake computations, each intake computation being a vector of compartments
/// computation, each compartment computation being a vector of concentrations.
///
class MultiConcentrationPrediction
{
public:
    /*
    bool streamToFile(const std::string _fileName)
    {
        std::ofstream ostrm(_fileName, std::ios::binary);
        if ((ostrm.rdstate() & std::ios_base::failbit) != 0) {
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

    */

    bool allocate(
            unsigned int _residualSize,
            CycleSize _nbPoints,
            TimeOffsets& _times,
            std::vector<Concentrations>& _values) const
    {
        _times.reserve(_nbPoints);
        for (unsigned int compartement = 0; compartement < _residualSize; compartement++) {
            _values[compartement].reserve(_nbPoints);
        }
        return true;
    }

    void appendConcentrations(TimeOffsets& _times, std::vector<Concentrations>& _values)
    {
        m_times.push_back(_times);
        m_values.push_back(_values);
    }


    MultiConcentrationPrediction* copy()
    {
        MultiConcentrationPrediction* result = new MultiConcentrationPrediction();
        result->m_times = this->m_times;
        result->m_values = this->m_values;
        return result;
    }

    // Very important to return const references, else a copy is made and it makes percentiles very slow
    const std::vector<TimeOffsets>& getTimes() const
    {
        return m_times;
    }
    const std::vector<std::vector<Concentrations> >& getValues() const
    {
        return m_values;
    }
    std::vector<std::vector<Concentrations> >& getModifiableValues()
    {
        return m_values;
    }

private:
    std::vector<TimeOffsets> m_times;
    ///
    /// \brief m_values
    ///
    /// The vector contains intakes calculations.
    /// Each intake calculation is a vector of compartments predictions,
    /// and each compartment prediction is a vector of concentrations.
    ///
    /// It would accessed like m_values[intakeIndex][compartmentIndex][timeIndex]
    ///
    std::vector<std::vector<Concentrations> > m_values;
};
typedef std::unique_ptr<MultiConcentrationPrediction> MultiConcentrationPredictionPtr;

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_MULTICONCENTRATIONPREDICTION_H
