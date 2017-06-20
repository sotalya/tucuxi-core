// TucuCli.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <time.h>

#include "TucuCore/onecompartmentextra.h"

int main()
{
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

	printf("Out residual = %f", outResiduals[0]);

    return 0;
}

