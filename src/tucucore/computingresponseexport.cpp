#include "computingresponseexport.h"

#include <iostream>
#include <fstream>
#include<iostream>
#include<sstream>
#include<fstream>
#include<iomanip>

namespace Tucuxi {
namespace Core {

ComputingResponseExport::ComputingResponseExport()
{

}


bool ComputingResponseExport::exportToFiles(const ComputingResponse &_computingResponse, std::string _filePath)
{

    for (auto &response : _computingResponse.getResponses()) {

        std::ofstream file;

        std::string fileName = _filePath + "/" + _computingResponse.getId() + "_" + response->getId() + ".dat";
        file.open(fileName);

        if (dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get()) != nullptr) {
            const Tucuxi::Core::SinglePredictionResponse* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get());

            double firstTime = prediction->getData()[0].m_start.toSeconds();
            for (auto &cycleData : prediction->getData()) {
                for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                    file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i] << "\t" << cycleData.m_concentrations[0][i] << std::endl;
                }
            }

        }
        else if (dynamic_cast<Tucuxi::Core::SinglePointsResponse*>(response.get()) != nullptr) {
            const Tucuxi::Core::SinglePointsResponse* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePointsResponse*>(response.get());

            double firstTime = prediction->m_times[0].toSeconds();

            for (size_t i = 0; i < prediction->m_concentrations[0].size(); i++) {
                file << (prediction->m_times[i].toSeconds() - firstTime) / 3600.0 << "\t" << prediction->m_concentrations[0][i] << std::endl;
            }
        }
        else if (dynamic_cast<Tucuxi::Core::PercentilesResponse*>(response.get()) != nullptr) {
            const Tucuxi::Core::PercentilesResponse* prediction =
                    dynamic_cast<Tucuxi::Core::PercentilesResponse*>(response.get());

            double firstTime = prediction->getPercentileData(0)[0].m_start.toSeconds();
            for(size_t cycleIndex = 0; cycleIndex < prediction->getPercentileData(0).size(); cycleIndex ++) {
                for (size_t i = 0; i < prediction->getPercentileData(0)[cycleIndex].m_concentrations[0].size(); i++) {
                    file <<  (prediction->getPercentileData(0)[cycleIndex].m_start.toSeconds() - firstTime) / 3600.0
                             + prediction->getPercentileData(0)[cycleIndex].m_times[0][i] << "\t";
                    for (size_t percIndex = 0; percIndex < prediction->getNbRanks(); percIndex ++) {
                        file  << prediction->getPercentileData(percIndex)[cycleIndex].m_concentrations[0][i] << "\t";
                    }
                    file << std::endl;
                }

            }
        }


        file.close();

    }

    return true;
}


} // namespace Core
} // namespace Tucuxi
