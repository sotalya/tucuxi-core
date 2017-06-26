// TucuCli.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <time.h>

#include "tucucore/onecompartmentextra.h"

#include "spdlog/spdlog.h"

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

	Tucuxi::Math::IntakeIntervalCalculator::Result res;
	Tucuxi::Math::OneCompartmentExtra calculator;

	time_t now;
	time(&now);

	Tucuxi::Math::Concentrations concentrations;
	Tucuxi::Math::Times times;
	Tucuxi::Math::IntakeEvent intakeEvent(now, 0, 0.2, 24, 1, 0, 250);
	Tucuxi::Math::ParameterList parameters;
	Tucuxi::Math::Residuals inResiduals;
	Tucuxi::Math::Residuals outResiduals;

	inResiduals.push_back(0);
	inResiduals.push_back(1);

	parameters.push_back(Tucuxi::Math::Parameter("CL", 14.3));
	parameters.push_back(Tucuxi::Math::Parameter("F", 1));
	parameters.push_back(Tucuxi::Math::Parameter("Ka", 0.609));
	parameters.push_back(Tucuxi::Math::Parameter("V", 347));

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

