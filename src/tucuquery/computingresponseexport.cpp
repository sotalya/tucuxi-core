#include "computingresponseexport.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Core {

ComputingResponseExport::ComputingResponseExport()
{

}


bool ComputingResponseExport::exportToFiles(const Query::ComputingQueryResponse &_computingQueryResponse, std::string _filePath)
{


    for (auto &response : _computingQueryResponse.getRequestResponses()) {

        std::ofstream file;

        std::string fileName = _filePath + "/" + _computingQueryResponse.getQueryId() + "_" + response.m_computingResponse->getId() + ".dat";
        file.open(fileName);
        if ((file.rdstate() & std::ostream::failbit) != 0) {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("The file {} could not be opened.", fileName);
            return false;
        }

        // We start by checking for adjustements, as AdjustmentResponse is a subclass of SinglePredictionResponse
        if (dynamic_cast<Tucuxi::Core::AdjustmentData*>(response.m_computingResponse.get()) != nullptr) {
            const Tucuxi::Core::AdjustmentData* prediction =
                    dynamic_cast<Tucuxi::Core::AdjustmentData*>(response.m_computingResponse.get());

            for (const auto &dosage : prediction->getAdjustments()) {
                for (const auto &timeRange : dosage.m_history.getDosageTimeRanges()) {

                    const DosageRepeat *repeat = dynamic_cast<const DosageRepeat *>(timeRange->getDosage());
                    const LastingDose *lastingDose = dynamic_cast<const LastingDose *>(repeat->getDosage());
                    file << lastingDose->getDose() << "\t" << lastingDose->getTimeStep().toHours() << "\t" << lastingDose->getInfusionTime().toHours() << "\t";
                }
                file << dosage.getGlobalScore() << "\t";

                for (const auto &targetEvaluation : dosage.m_targetsEvaluation) {
                    file << targetEvaluation.getValue() << "\t";
                }


                for (const auto& p : dosage.m_data[0].m_parameters) {
                    file << p.m_value << "\t";
                }
                file << std::endl;

                double firstTime = dosage.m_data[0].m_start.toSeconds();
                size_t dataIndex = 0;
                for (auto &cycleData : dosage.m_data) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i];
                        if ((i != cycleData.m_concentrations[0].size() - 1) || (dataIndex != dosage.m_data.size() - 1)) {
                            file << "\t";
                        }
                    }
                    dataIndex ++;
                }
                file << std::endl;

                dataIndex = 0;
                for (auto &cycleData : dosage.m_data) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << cycleData.m_concentrations[0][i];
                        if ((i != cycleData.m_concentrations[0].size() - 1) || (dataIndex != dosage.m_data.size() - 1)) {
                            file << "\t";
                        }
                    }
                    dataIndex ++;
                }
                file << std::endl;

            }

        }
        else if (dynamic_cast<Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse.get()) != nullptr) {
            const Tucuxi::Core::SinglePredictionData* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse.get());

            if (!prediction->getData().empty()) {
                double firstTime = prediction->getData()[0].m_start.toSeconds();
                for (auto &cycleData : prediction->getData()) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i] << "\t" << cycleData.m_concentrations[0][i] << std::endl;
                    }
                }
            }

        }
        else if (dynamic_cast<Tucuxi::Core::SinglePointsData*>(response.m_computingResponse.get()) != nullptr) {
            const Tucuxi::Core::SinglePointsData* prediction =
                    dynamic_cast<Tucuxi::Core::SinglePointsData*>(response.m_computingResponse.get());

            double firstTime = prediction->m_times[0].toSeconds();

            for (size_t i = 0; i < prediction->m_concentrations[0].size(); i++) {
                file << (prediction->m_times[i].toSeconds() - firstTime) / 3600.0 << "\t" << prediction->m_concentrations[0][i] << std::endl;
            }
        }
        else if (dynamic_cast<Tucuxi::Core::PercentilesData*>(response.m_computingResponse.get()) != nullptr) {
            const Tucuxi::Core::PercentilesData* prediction =
                    dynamic_cast<Tucuxi::Core::PercentilesData*>(response.m_computingResponse.get());

            double firstTime = prediction->getPercentileData(0)[0].m_start.toSeconds();
            for(size_t cycleIndex = 0; cycleIndex < prediction->getPercentileData(0).size(); cycleIndex ++) {
                for (size_t i = 0; i < prediction->getPercentileData(0)[cycleIndex].m_concentrations[0].size(); i++) {
                    file <<  (prediction->getPercentileData(0)[cycleIndex].m_start.toSeconds() - firstTime) / 3600.0
                             + prediction->getPercentileData(0)[cycleIndex].m_times[0][i] << "\t";
                    for (unsigned int percIndex = 0; percIndex < prediction->getNbRanks(); percIndex ++) {
                        file  << prediction->getPercentileData(percIndex)[cycleIndex].m_concentrations[0][i];
                        if (percIndex != prediction->getNbRanks() - 1) {
                            file << "\t";
                        }
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
