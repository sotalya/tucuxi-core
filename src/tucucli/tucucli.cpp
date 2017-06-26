/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <stdio.h>
#include <time.h>

#include "spdlog/spdlog.h"

#include "tucucore/onecompartmentextra.h"

/// \defgroup TucuCli Tucuxi Console application
/// \brief The Tucuxi console application
/// 
/// The Tucuxi console application offers a simple command line interface to 
/// launch prediction and suggestion algorithms.
/// 
/// This application is intended mainly to run automated test scripts

int main()
{
	//Multithreaded console logger(with color support)
	auto console = spdlog::stdout_color_mt("console");
	console->info("Welcome to spdlog!");
	console->info("An info message example {}.", 1);

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logfile", 1024 * 1024 * 5, 3));
	auto combined_logger = std::make_shared<spdlog::logger>("name", begin(sinks), end(sinks));
	spdlog::register_logger(combined_logger);

	combined_logger->info("Welcome to spdlog!");

	Tucuxi::Core::IntakeIntervalCalculator::Result res;
	Tucuxi::Core::OneCompartmentExtra calculator;

	time_t now;
	time(&now);

	Tucuxi::Core::Concentrations concentrations;
	Tucuxi::Core::Times times;
	Tucuxi::Core::IntakeEvent intakeEvent(now, 0, 0.2, 24, 1, 0, 250);
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

    return 0;
}

