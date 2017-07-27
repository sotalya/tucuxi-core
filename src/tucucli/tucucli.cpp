/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <stdio.h>
#include <time.h>

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucore/definitions.h"
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

    logHelper.info("********************************************************");
    logHelper.info("Tucuxi console application is starting up...");
    
    Tucuxi::Core::IntakeIntervalCalculator::Result res;
    Tucuxi::Core::OneCompartmentExtra calculator;

    DateTime now;

    Tucuxi::Core::Concentrations concentrations;
    Tucuxi::Core::TimeOffsets times;
    Tucuxi::Core::IntakeEvent intakeEvent(now, 0s, 0.2, 24h, Tucuxi::Core::RouteOfAdministration::INTRAVASCULAR, 0s, 250);
    Tucuxi::Core::ParameterList parameters;
    Tucuxi::Core::Residuals inResiduals;
    Tucuxi::Core::Residuals outResiduals;

    inResiduals.push_back(0);
    inResiduals.push_back(1);

    parameters.push_back(Tucuxi::Core::Parameter("CL", 14.3));
    parameters.push_back(Tucuxi::Core::Parameter("F", 1));
    parameters.push_back(Tucuxi::Core::Parameter("Ka", 0.609));
    parameters.push_back(Tucuxi::Core::Parameter("V", 347));

    res = calculator.calculateIntakePoints(
        concentrations,
        times,
        intakeEvent,
        parameters,
        inResiduals,
        250,
        outResiduals,
        true);

    printf("Out residual = %f\n", outResiduals[0]);

    logHelper.info("Tucuxi console application is exiting...");
    return 0;
}

