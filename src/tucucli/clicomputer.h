#ifndef CLICOMPUTER_H
#define CLICOMPUTER_H

#include <string>

///
/// \brief The CliComputer class
///
class CliComputer
{
public:
    CliComputer();

    ///
    /// \brief Performs a complete calculation
    /// \param drugPath The path to the drug files
    /// \param inputFileName The path to the file containing all required input data
    /// \param outputPath The path to the folder that will contain the output files
    /// This method is the single entry point exploited by the Command Line Interface
    ///
    void compute(std::string drugPath,
                 std::string inputFileName,
                 std::string outputPath);
};

#endif // CLICOMPUTER_H
