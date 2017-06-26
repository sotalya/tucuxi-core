/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_MATH_ONECOMPARTMENTEXTRA_H
#define TUCUXI_MATH_ONECOMPARTMENTEXTRA_H

#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

class OneCompartmentExtra : public IntakeIntervalCalculator
{
public:
	OneCompartmentExtra();

protected:
	virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
	virtual void prepareComputations(const IntakeEvent& _intakeEvent, const ParameterList& _parameters) override;
	virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterList& _parameters, Eigen::VectorXd& _times) override;
	virtual void computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) override;

private:
	Float m_D;
	Float m_CL;
	Float m_F;
	Float m_Ka;
	Float m_V;
	Float m_Ke;
};

}
}

#endif // TUCUXI_MATH_ONECOMPARTMENTEXTRA_H