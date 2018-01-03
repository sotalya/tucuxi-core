/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <stdio.h>
#include <time.h>

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/licensemanager.h"
#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/onecompartmentextra.h"

using namespace std::chrono_literals;

/// \defgroup TucuCli Tucuxi Console application
/// \brief The Tucuxi console application
///
/// The Tucuxi console application offers a simple command line interface to
/// launch prediction and suggestion algorithms.
///
/// This application is intended mainly to run automated test scripts

int main(int argc, char** argv)
{
    // Get application folder
    std::string appFolder = Tucuxi::Common::Utils::getAppFolder(argv);

    Tucuxi::Common::LoggerHelper::init(appFolder + "/tucucli.log");
    Tucuxi::Common::LoggerHelper logHelper;
/*
    if (Tucuxi::Common::LicenseManager::checkLicenseFile("c:\\Projects\\Tucuxi\\bin\\license2.txt") != Tucuxi::Common::LicenseError::VALID_LICENSE)
    {
        return -1;
    }
*/

    logHelper.info("********************************************************");
    logHelper.info("Tucuxi console application is starting up...");

    Tucuxi::Core::IntakeIntervalCalculator::Result res;
    Tucuxi::Core::OneCompartmentExtraMacro calculator;

    DateTime now;
    int nbPoints = 251;
    bool isAll = false;

    std::vector<Tucuxi::Core::Concentrations> concentrations;
    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(now, 0s, 400, 24h, Tucuxi::Core::AbsorptionModel::INTRAVASCULAR, 0s, nbPoints);
    Tucuxi::Core::ParameterDefinitions parameterDefs;
    Tucuxi::Core::Residuals inResiduals;
    Tucuxi::Core::Residuals outResiduals1, outResiduals2;

    inResiduals.push_back(0);
    inResiduals.push_back(0);

    parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Cl", 15.6, Tucuxi::Core::ParameterVariabilityType::Additive)));
    parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::Additive)));
    parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::Additive)));
    parameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V", 347, Tucuxi::Core::ParameterVariabilityType::Additive)));
    Tucuxi::Core::ParameterSetEvent parameters(DateTime(), parameterDefs);

    res = calculator.calculateIntakePoints(
        concentrations,
        times,
        intakeEvent,
        parameters,
        inResiduals,
        nbPoints,
	isAll,
        outResiduals1,
        true);
    printf("Out residual = %f\n", outResiduals1[0]);
    printf("Out residual = %f\n", outResiduals1[1]);

    res = calculator.calculateIntakeSinglePoint(
        concentrations,
        intakeEvent,
        parameters,
        inResiduals,
        nbPoints,
	isAll,
        outResiduals2);
    printf("Out residual = %f\n", outResiduals2[0]);
    printf("Out residual = %f\n", outResiduals2[1]);

    for (int i = 0; i < 2; i++) {
        if (abs(outResiduals1[i]/outResiduals2[i]-1) > 0.000001) {
            logHelper.info("Error: Mismatch in computed residuals: {} != {}", outResiduals1[i], outResiduals2[i]);
        }
    }

    logHelper.info("Tucuxi console application is exiting...");
    return 0;
}
