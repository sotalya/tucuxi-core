/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <Eigen/Dense>

#include "tucucommon/loggerhelper.h"
#include "tucucore/pkmodels/twocompartmentinfusion.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

#if 0
#define DEBUG
#endif

TwoCompartmentInfusionMicro::TwoCompartmentInfusionMicro() : IntakeIntervalCalculatorBase<2, TwoCompartmentInfusionExponentials> (new PertinentTimesCalculatorInfusion())
{
}

bool TwoCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
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
    logHelper.debug("m_V1: {}", m_V1);
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

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(m_Ke > 0, "The Ke is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_Ke), "The Ke is NaN.");
    bOK &= checkValue(!std::isinf(m_Ke), "The Ke is Inf.");
    bOK &= checkValue(m_K12 > 0, "The K12 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K12), "The K12 is NaN.");
    bOK &= checkValue(!std::isinf(m_K12), "The K12 is Inf.");
    bOK &= checkValue(m_K21 > 0, "The K21 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_K21), "The K21 is NaN.");
    bOK &= checkValue(!std::isinf(m_K21), "The K21 is Inf.");
    bOK &= checkValue(m_Divider != 0.0, "Divide by zero.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

    return bOK;
}



void TwoCompartmentInfusionMicro::computeExponentials(Eigen::VectorXd& _times)
{
    setExponentials(Exponentials::Alpha, (-m_Alpha * _times).array().exp());
    setExponentials(Exponentials::Beta, (-m_Beta * _times).array().exp());
    setExponentials(Exponentials::AlphaInf, (m_Alpha * _times).array().exp());
    setExponentials(Exponentials::BetaInf, (m_Beta * _times).array().exp());
    setExponentials(Exponentials::BetaInf2, (-2 * m_Beta * _times).array().exp());
    setExponentials(Exponentials::Root, (m_RootK * _times).array().exp());

    Eigen::VectorXd times(_times.size());
    times = _times - m_Tinf * Eigen::VectorXd::Ones(_times.size());
    setExponentials(Exponentials::AlphaPostInf,  (-m_Alpha * times).array().exp());
    setExponentials(Exponentials::BetaPostInf, (-m_Beta * times).array().exp());
}


bool TwoCompartmentInfusionMicro::computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);

    int forcesize = static_cast<int>(std::min(ceil(m_Tinf/m_Int * m_NbPoints), ceil(m_NbPoints)));

    // Compute concentrations
    compute(_inResiduals, forcesize, concentrations1, concentrations2);

    // Return final residuals of comp1 and comp2
    _outResiduals[firstCompartment] = concentrations1[m_NbPoints - 1];
    _outResiduals[secondCompartment] = concentrations2[m_NbPoints - 1];

    // Return concentrations of comp1
    _concentrations[firstCompartment].assign(concentrations1.data(), concentrations1.data() + concentrations1.size());
    if (_isAll == true) {
	_concentrations[secondCompartment].assign(concentrations2.data(), concentrations2.data() + concentrations2.size());	
    }

    // Check output
    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}


bool TwoCompartmentInfusionMicro::computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
{
    Eigen::VectorXd concentrations1, concentrations2;
    int firstCompartment = static_cast<int>(Compartments::First);
    int secondCompartment = static_cast<int>(Compartments::Second);
    int atTime = static_cast<int>(SingleConcentrations::AtTime);
    int atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

    int forcesize = 0;

    // remove following code after verification with unit test
#if 0
    Eigen::VectorXd& alphaLogV = logs(Exponentials::Alpha); 
    Eigen::VectorXd& betaLogV = logs(Exponentials::Beta); 
    Eigen::VectorXd& alphaInfLogV = logs(Exponentials::AlphaInf); 
    Eigen::VectorXd& betaInfLogV = logs(Exponentials::BetaInf); 
    Eigen::VectorXd& betaInf2LogV = logs(Exponentials::BetaInf2); 
    Eigen::VectorXd& rootLogV = logs(Exponentials::Root); 

    Concentration resid1 = _inResiduals[0];
    Concentration resid2 = _inResiduals[1];

    Value deltaD = (m_D / m_V1) / m_Tinf; 

    Concentration residInf1 =
        (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K21
        * (std::exp(-m_Beta * m_Tinf) * (-m_K12 - m_K21 + m_Ke - m_RootK) 
            + std::exp(-2 * m_Beta * m_Tinf) * (m_K12 + m_K21 - m_Ke + m_RootK)
            + std::exp(m_RootK*m_Tinf - m_Alpha*m_Tinf) * (m_K12 + m_K21 - m_Ke - m_RootK)
            + std::exp(-m_Alpha*m_Tinf - m_Beta*m_Tinf) * (-m_K12 - m_K21 + m_Ke + m_RootK))
	) / m_Divider;

    Concentration residInf2 = 
        (2 * deltaD * std::exp(m_Beta * m_Tinf) * m_K12
	* (std::exp(-m_Beta * m_Tinf) * (-m_SumK - m_RootK)
	    + std::exp(-2 * m_Beta * m_Tinf) * (m_SumK + m_RootK)
            + std::exp(m_RootK * m_Tinf - m_Alpha * m_Tinf) * (m_SumK - m_RootK)
            + std::exp(-m_Alpha * m_Tinf - m_Beta * m_Tinf) * (-m_SumK + m_RootK))
        ) / m_Divider;

    Value A = ((m_K12 - m_K21 + m_Ke + m_RootK) * resid1) - (2 * m_K21 * resid2);
    Value B = ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid1) + (2 * m_K21 * resid2);
    Value A2 = (-2 * m_K12 * resid1) + ((-m_K12 + m_K21 - m_Ke + m_RootK) * resid2);
    Value BB2 = (2 * m_K12 * resid1) + ((m_K12 - m_K21 + m_Ke + m_RootK) * resid2);
    Value AInf = -m_K12 - m_K21 + m_Ke - m_RootK;
    Value BInf = m_K12 + m_K21 - m_Ke - m_RootK;
    Value BPostInf = (-m_K12 + m_K21 - m_Ke + m_RootK)*residInf1 + 2*m_K21*residInf2;
    Value APostInf = (m_K12 - m_K21 + m_Ke + m_RootK)*residInf1 - 2*m_K21*residInf2;

    // Calculate concentrations for comp1 and comp2
    concentrations1 = ((A * alphaLogV) + (B * betaLogV)) / (2 * m_RootK);
    concentrations2 = ((A2 * alphaLogV) + (BB2 * betaLogV)) / (2 * m_RootK);
    
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

    for (int i = 0; i < 2; i++)
    {
	    printf("alphaLogV[%d]: %f\n", i, logs(Exponentials::Alpha)[i]);
	    printf("_concentrations[%d]: %f\n", i, _concentrations[i]);
	    printf("_outResiduals[%d]: %f\n", i, _outResiduals[i]);
    }

#else
    if (_atTime <= m_Tinf) {
	    forcesize = 1;
    }

    // Compute concentrations
    compute(_inResiduals, forcesize, concentrations1, concentrations2);

    // Return concentraions (computation with atTime (current time))
    _concentrations[firstCompartment].push_back(concentrations1[atTime]);
    if (_isAll == true) {
	_concentrations[secondCompartment].push_back(concentrations2[atTime]);
    }
    
    // Return final residual of comp1 and comp2
    // TODO: check equation... 
    // In case of ezechiel, when forcesize = 1,
    // concentrations1[1] = concentrations1[0] + (APostInf * alphaLogV(1) + BPostInf * betaLogV(1)) / (2 * m_RootK)
    // concentrations2[1] = (A2 * alphaLogV(1) + BB2 * betaLogV(1)) / (2 * m_RootK)
    _outResiduals[firstCompartment] = concentrations1[atEndInterval];
    _outResiduals[secondCompartment] = concentrations2[atEndInterval];
#endif

    // Check output
    bool bOK = checkValue(_outResiduals[firstCompartment] >= 0, "The concentration1 is negative.");
    bOK &= checkValue(_outResiduals[secondCompartment] >= 0, "The concentration2 is negative.");

    return bOK;
}

TwoCompartmentInfusionMacro::TwoCompartmentInfusionMacro()
{
}

bool TwoCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkValue(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    Value cl = _parameters.getValue(ParameterId::CL);
    Value q = _parameters.getValue(ParameterId::Q);
    m_V1 = _parameters.getValue(ParameterId::V1);
    Value v2 = _parameters.getValue(ParameterId::V2);
    m_Ke = cl / m_V1;
    m_K12 = q / m_V1;
    m_K21 = q / v2;
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
    logHelper.debug("cl: {}", cl);
    logHelper.debug("q: {}", q);
    logHelper.debug("m_V1: {}", m_V1);
    logHelper.debug("v2: {}", v2);
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

    bool bOK = checkValue(m_D >= 0, "The dose is negative.");
    bOK &= checkValue(!std::isnan(m_D), "The dose is NaN.");
    bOK &= checkValue(!std::isinf(m_D), "The dose is Inf.");
    bOK &= checkValue(cl > 0, "The clearance is not greater than zero.");
    bOK &= checkValue(!std::isnan(cl), "The CL is NaN.");
    bOK &= checkValue(!std::isinf(cl), "The CL is Inf.");
    bOK &= checkValue(q > 0, "The Q is not greater than zero.");
    bOK &= checkValue(!std::isnan(q), "The Q is NaN.");
    bOK &= checkValue(!std::isinf(q), "The Q is Inf.");
    bOK &= checkValue(m_V1 > 0, "The volume1 is not greater than zero.");
    bOK &= checkValue(!std::isnan(m_V1), "The V1 is NaN.");
    bOK &= checkValue(!std::isinf(m_V1), "The V1 is Inf.");
    bOK &= checkValue(v2 > 0, "The volume2 is not greater than zero.");
    bOK &= checkValue(!std::isnan(v2), "The V2 is NaN.");
    bOK &= checkValue(!std::isinf(v2), "The V2 is Inf.");
    bOK &= checkValue(m_Divider != 0.0, "Divide by zero.");
    bOK &= checkValue(m_Tinf >= 0, "The infusion time is zero or negative.");
    bOK &= checkValue(m_NbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkValue(m_Int > 0, "The interval time is not greater than zero.");
    bOK &= checkValue(m_Alpha >= 0, "Alpha is negative.");
    bOK &= checkValue(m_Beta >= 0, "Beta is negative.");

    return bOK;
}

}
}

