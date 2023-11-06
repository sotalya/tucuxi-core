//@@license@@

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
            const std::string& _inputFileName,
            const std::string& _outputFileName,
            const std::string& _dataFilePath,
            const std::string& _tqfOutputFileName = "");
};


#endif // CLICOMPUTER_H
