#ifndef CLICOMPUTER_H
#define CLICOMPUTER_H

#include <string>
#include "tucuquery/querystatus.h"

///
/// \brief The CliComputer class
///
class CliComputer
{
public:
    CliComputer();

    ///
    /// \brief Performs a complete calculation
    /// \param _drugPath The path to the drug files
    /// \param _inputFileName The path to the file containing all required input data
    /// \param _outputPath The path to the folder that will contain the output files
    /// \return an error number. 0 if everything went well
    /// This method is the single entry point exploited by the Command Line Interface
    ///
    QueryStatus compute(std::string _drugPath,
                 std::string _inputFileName,
                 std::string _outputPath);
};

#endif // CLICOMPUTER_H
