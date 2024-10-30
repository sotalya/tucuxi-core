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


#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "computingresponseexport.h"

#include "tucucommon/loggerhelper.h"

namespace Tucuxi {
namespace Core {

ComputingResponseExport::ComputingResponseExport() = default;


bool ComputingResponseExport::exportToFiles(
        const Query::ComputingQueryResponse& _computingQueryResponse, const std::string& _filePath)
{


    for (auto& response : _computingQueryResponse.getRequestResponses()) {

        std::ofstream file;

        std::string fileName = _filePath + "/" + _computingQueryResponse.getQueryId() + "_"
                               + response.m_computingResponse->getId() + ".dat";
        file.open(fileName);
        if ((file.rdstate() & std::ostream::failbit) != 0) {
            Tucuxi::Common::LoggerHelper logHelper;
            logHelper.error("The file {} could not be opened.", fileName);
            return false;
        }

        // We start by checking for adjustments, as AdjustmentResponse is a subclass of SinglePredictionResponse
        if (dynamic_cast<const Tucuxi::Core::AdjustmentData*>(response.m_computingResponse->getData()) != nullptr) {
            const Tucuxi::Core::AdjustmentData* prediction =
                    dynamic_cast<const Tucuxi::Core::AdjustmentData*>(response.m_computingResponse->getData());

            for (const auto& dosage : prediction->getAdjustments()) {
                for (const auto& timeRange : dosage.m_history.getDosageTimeRanges()) {

                    const DosageRepeat* repeat = dynamic_cast<const DosageRepeat*>(timeRange->getDosage());
                    const LastingDose* lastingDose = dynamic_cast<const LastingDose*>(repeat->getDosage());
                    file << lastingDose->getDose() << "\t" << lastingDose->getTimeStep().toHours() << "\t"
                         << lastingDose->getInfusionTime().toHours() << "\t";
                }
                file << dosage.getGlobalScore() << "\t";

                for (const auto& targetEvaluation : dosage.m_targetsEvaluation) {
                    file << targetEvaluation.getValue() << "\t";
                }


                for (const auto& p : dosage.getData()[0].m_parameters) {
                    file << p.m_value << "\t";
                }
                file << std::endl;

                double firstTime = dosage.getData()[0].m_start.toSeconds();
                size_t dataIndex = 0;
                for (auto& cycleData : dosage.getData()) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i];
                        if ((i != cycleData.m_concentrations[0].size() - 1)
                            || (dataIndex != dosage.getData().size() - 1)) {
                            file << "\t";
                        }
                    }
                    dataIndex++;
                }
                file << std::endl;

                dataIndex = 0;
                for (auto& cycleData : dosage.getData()) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << cycleData.m_concentrations[0][i];
                        if ((i != cycleData.m_concentrations[0].size() - 1)
                            || (dataIndex != dosage.getData().size() - 1)) {
                            file << "\t";
                        }
                    }
                    dataIndex++;
                }
                file << std::endl;
            }
        }
        else if (
                dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse->getData())
                != nullptr) {
            const Tucuxi::Core::SinglePredictionData* prediction =
                    dynamic_cast<const Tucuxi::Core::SinglePredictionData*>(response.m_computingResponse->getData());

            if (!prediction->getData().empty()) {
                double firstTime = prediction->getData()[0].m_start.toSeconds();
                for (auto& cycleData : prediction->getData()) {
                    for (size_t i = 0; i < cycleData.m_concentrations[0].size(); i++) {
                        file << (cycleData.m_start.toSeconds() - firstTime) / 3600.0 + cycleData.m_times[0][i] << "\t"
                             << cycleData.m_concentrations[0][i] << std::endl;
                    }
                }
            }
        }
        else if (
                dynamic_cast<const Tucuxi::Core::SinglePointsData*>(response.m_computingResponse->getData())
                != nullptr) {
            const Tucuxi::Core::SinglePointsData* prediction =
                    dynamic_cast<const Tucuxi::Core::SinglePointsData*>(response.m_computingResponse->getData());

            double firstTime = prediction->m_times[0].toSeconds();

            for (size_t i = 0; i < prediction->m_concentrations[0].size(); i++) {
                file << (prediction->m_times[i].toSeconds() - firstTime) / 3600.0 << "\t"
                     << prediction->m_concentrations[0][i] << std::endl;
            }
        }
        else if (
                dynamic_cast<const Tucuxi::Core::PercentilesData*>(response.m_computingResponse->getData())
                != nullptr) {
            const Tucuxi::Core::PercentilesData* prediction =
                    dynamic_cast<const Tucuxi::Core::PercentilesData*>(response.m_computingResponse->getData());

            double firstTime = prediction->getPercentileData(0)[0].m_start.toSeconds();
            for (size_t cycleIndex = 0; cycleIndex < prediction->getPercentileData(0).size(); cycleIndex++) {
                for (size_t i = 0; i < prediction->getPercentileData(0)[cycleIndex].m_concentrations[0].size(); i++) {
                    file << (prediction->getPercentileData(0)[cycleIndex].m_start.toSeconds() - firstTime) / 3600.0
                                    + prediction->getPercentileData(0)[cycleIndex].m_times[0][i]
                         << "\t";
                    for (unsigned int percIndex = 0; percIndex < prediction->getNbRanks(); percIndex++) {
                        file << prediction->getPercentileData(percIndex)[cycleIndex].m_concentrations[0][i];
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
