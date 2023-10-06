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


#ifndef CLICOMPUTER_H
#define CLICOMPUTER_H

#include <cstdint>
#include <string>

#include "tucuquery/querystatus.h"

#ifdef WIN32
const char SEPARATOR = '\\';
const uint8_t NUMBER = 1;
#else
const char SEPARATOR = '/';
const uint8_t NUMBER = 0;
#endif


///
/// \brief The CliComputer class
///
class CliComputer
{
public:
    CliComputer();

    ///
    /// \brief Performs a complete calculation
    /// \param _inputFileName The path to the file containing all required input data
    /// \param _outputPath The path to the folder that will contain the output files
    /// \return an error number. 0 if everything went well
    /// This method is the single entry point exploited by the Command Line Interface
    ///
    Tucuxi::Query::QueryStatus compute(
            const std::string& _inputFileName, const std::string& _outputFileName, const std::string& _dataFilePath);
};


#endif // CLICOMPUTER_H
