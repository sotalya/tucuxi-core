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


bool ComputingResponseExport::exportToFiles(const ComputingResponse &computingResponse, std::string filePath)
{

    for (auto &response : computingResponse.getResponses()) {

        std::ofstream file;

        std::string fileName = filePath + "/" + computingResponse.getId() + "_" + response->getId() + ".dat";
        file.open(fileName);

        if (dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get())) {
            const Tucuxi::Core::SinglePredictionResponse* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePredictionResponse*>(response.get());

            double firstTime = prediction->getData()[0].m_start.toSeconds();
            for (auto &cycleData : prediction->getData()) {
                for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                    file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i] << "\t" << cycleData.m_concentrations[0][i] << std::endl;
                }
            }

        }


        file.close();

    }

    return true;
}


} // namespace Core
} // namespace Tucuxi
