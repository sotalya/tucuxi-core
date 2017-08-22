/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/twocompartmentinfusion.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 1
#define DEBUG
#endif

TwoCompartmentInfusion::TwoCompartmentInfusion()
{
}

bool TwoCompartmentInfusion::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    bool bOK = true;

    if(!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4."))
	    return false;

    m_D = _intakeEvent.getDose() * 1000;
    m_Cl = _parameters.getValue(0);
    m_Q = _parameters.getValue(1);
    m_V1 = _parameters.getValue(2);
    m_V2 = _parameters.getValue(3);
    m_Ke = m_Cl / m_V1;
    m_K12 = m_Q / m_V1;
    m_K21 = m_Q / m_V2;
    m_SumK = m_Ke + m_K12 + m_K21;
    m_RootK = std::sqrt((m_SumK * m_SumK) - (4 * m_K21 * m_Ke));
    m_Divider = m_RootK * (-m_SumK + m_RootK) * (m_SumK + m_RootK);
    m_Alpha = (m_SumK + m_RootK)/2;
    m_Beta = (m_SumK - m_RootK)/2;
    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_Int = (_intakeEvent.getInterval()).toHours();
    m_NbPoints = _intakeEvent.getNbPoints();
    
#ifdef DEBUG
    Tucuxi::Common::LoggerHelper logHelper;

    logHelper.debug("<<Input Values>>");
    logHelper.debug("m_D: {}", m_D);
    logHelper.debug("m_Cl: {}", m_Cl);
    logHelper.debug("m_Q: {}", m_Q);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("m_V2: {}", m_V2);
    logHelper.debug("m_Ke: {}", m_Ke);
    logHelper.debug("m_K12: {}", m_K12);
    logHelper.debug("m_K21: {}", m_K21);
    logHelper.debug("m_SumK: {}", m_SumK);
    logHelper.debug("m_RootK: {}", m_RootK);
    logHelper.debug("m_Divider: {}", m_Divider);
    logHelper.debug("m_Alpha: {}", m_Alpha);
    logHelper.debug("m_Beta: {}", m_Beta);
    logHelper.debug("m_Tinf: {}", m_Tinf);
    logHelper.debug("m_Int: {}", m_Int);
#endif

    bOK &= checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_Cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(m_Cl), "The CL is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_V2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(m_V2), "The V2 is Inf.");
    bOK &= checkValue(m_Divider != 0.0, "Divide by zero.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is negative.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");

    return bOK;
}



void TwoCompartmentInfusion::computeLogarithms(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, Eigen::VectorXd& _times)
{
    setLogs(Logarithms::Alpha, (-m_Alpha * _times).array().exp());
    setLogs(Logarithms::Beta, (-m_Beta * _times).array().exp());
    setLogs(Logarithms::AlphaInf, (m_Alpha * _times).array().exp());
    setLogs(Logarithms::BetaInf, (m_Beta * _times).array().exp());
    setLogs(Logarithms::BetaInf2, (-2 * m_Beta * _times).array().exp());
    setLogs(Logarithms::Root, (m_RootK * _times).array().exp());
}


bool TwoCompartmentInfusion::computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;

    int forcesize = static_cast<int>(std::min(ceil(m_Tinf/m_Int * m_NbPoints), ceil(m_NbPoints)));

    // Compute concentrations
    compute(_inResiduals, forcesize, concentrations1, concentrations2);

    // Return final residuals of comp1 and comp2
    _outResiduals.push_back(concentrations1[m_NbPoints - 1]);
    _outResiduals.push_back(concentrations2[m_NbPoints - 1]);

    // Return concentrations of comp1 and comp2
    _concentrations.assign(concentrations1.data(), concentrations1.data() + concentrations1.size());

    // Check output
    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}


bool TwoCompartmentInfusion::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;

    int forcesize = 0;

    // remove following code after verification with unit test
#if 0
    if (_atTime <= m_Tinf)
    {
	// During infusion
	Value p1p1 = 2 * deltaD * m_K21 * betaInfLogV(0);
	Value p1p2 = AInf * (betaLogV(0) - betaInf2LogV(0));
	Value p1p3 = BInf * ((rootLogV(0) / alphaInfLogV(0)) - (alphaLogV(0) / betaInfLogV(0)));
	Value p2p1 = 2 * deltaD * m_K12 * betaInfLogV(0);
	Value p2p2 = 
	    betaLogV(0) 
	    * (-m_SumK - m_RootK) 
	    + betaInf2LogV(0) * (m_SumK + m_RootK)
	    + rootLogV(0)/alphaInfLogV(0) * (m_SumK - m_RootK) 
	    + alphaLogV(0)/betaInfLogV(0) * (-m_SumK + m_RootK);
	
	concentrations1(0) = 
	    concentrations1(0) + ((p1p1 * (p1p2 + p1p3)) / m_Divider);
	concentrations2(0) = 
	    concentrations2(0) + ((p2p1 * p2p2) / m_Divider);

    }
    else
    {
	// After infusion
	concentrations1(0) = 
	    concentrations1(0) 
	    + (APostInf * alphaLogV(0) + BPostInf * betaLogV(0)) / (2 * m_RootK);
	
	BB2 = 2 * m_K12 * residInf1 + (m_K12 - m_K21 + m_Ke + m_RootK)*residInf2;
	A2 = -2 * m_K12 * residInf1 + (-m_K12 + m_K21 - m_Ke + m_RootK)*residInf2;
	concentrations2(0) = 
	    concentrations2(0) 
	    + (A2 * alphaLogV(0) + BB2 * betaLogV(0)) / (2 * m_RootK);
    }

    // return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);
    _concentrations.push_back(concentrations2[0]);
    
    // return final residual of comp1 and comp2
    _outResiduals.push_back
    (concentrations1[0] + (APostInf * alphaLogV(1) + BPostInf * betaLogV(1)) / (2 * m_RootK));
    // TODO: confirm the equation whether need to add concentrations2 or not.
    // In case of ezechiel, finalResiduals[1] = (A2 * alphalogf + BB2 * betalogf) / (2*root);
    _outResiduals.push_back((A2 * alphaLogV(1) + BB2 * betaLogV(1)) / (2 * m_RootK));
#else
    if (_atTime <= m_Tinf) {
	    forcesize = 1;
    }

    // Compute concentrations
    compute(_inResiduals, forcesize, concentrations1, concentrations2);

    // Return concentraions (computation with atTime (current time))
    _concentrations.push_back(concentrations1[0]);
    _concentrations.push_back(concentrations2[0]);
    
    // Return final residual of comp1 and comp2
    // TODO: check equation... 
    // In case of ezechiel, when forcesize = 1,
    // concentrations1[1] = concentrations1[0] + (APostInf * alphaLogV(1) + BPostInf * betaLogV(1)) / (2 * m_RootK)
    // concentrations2[1] = (A2 * alphaLogV(1) + BB2 * betaLogV(1)) / (2 * m_RootK)
    _outResiduals.push_back(concentrations1[1]);
    _outResiduals.push_back(concentrations2[1]);
#endif

    // Check output
    bool bOK = checkValue(_outResiduals[0] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[1] >= 0, "The concentration2 is negative.");

    return bOK;
}

}
}

