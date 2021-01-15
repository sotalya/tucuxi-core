#include "rkmichaelismentenenzyme.h"
#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkMichaelisMentenEnzyme::RkMichaelisMentenEnzyme() : IntakeIntervalCalculatorRK4Base<3, RkMichaelisMentenEnzyme> (new PertinentTimesCalculatorStandard())
{
}


RkMichaelisMentenEnzymeExtra::RkMichaelisMentenEnzymeExtra() : RkMichaelisMentenEnzyme()
{
}

std::vector<std::string> RkMichaelisMentenEnzymeExtra::getParametersId()
{
    return {"V", "Km", "Vmax", "F", "Ka", "Kenz", "Emax", "ECmid", "EDmid", "DoseMid", "Fmax", "NN", "MTT", "AllmCL"};
}

Value toUgL(Value _v) { return _v * 1.0;}

bool RkMichaelisMentenEnzymeExtra::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 11, "The number of parameters should be equal to 10.")) {
        return false;
    }

    m_D = toUgL(_intakeEvent.getDose());// * 1000.0;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = toUgL(_parameters.getValue(ParameterId::Km));// * 1000.0;
    m_Vmax = toUgL(_parameters.getValue(ParameterId::Vmax));// * 1000.0;
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_AllmCL = _parameters.getValue(ParameterId::AllmCL);

    m_Kenz = _parameters.getValue(ParameterId::Kenz);
    m_Emax = _parameters.getValue(ParameterId::Emax);
    m_ECmid = toUgL(_parameters.getValue(ParameterId::ECmid));// * 1000.0;
    m_EDmid = toUgL(_parameters.getValue(ParameterId::EDmid));// * 1000.0;
    m_DoseMid = toUgL(_parameters.getValue(ParameterId::DoseMid));// * 1000.0;
    m_Fmax = _parameters.getValue(ParameterId::Fmax);
    m_NN = _parameters.getValue(ParameterId::NN);
    m_MTT = _parameters.getValue(ParameterId::MTT);

    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();


    m_ktr = (m_NN + 1.0) / m_MTT;
    // Logarithm of the approximation to the gamma function
    auto l = 0.9189385 + (m_NN + 0.5)*std::log(m_NN) - m_NN + std::log(1.0 + 1.0/(12.0 * m_NN));

    Value tvbio;

    // The formula for tvbio has a discontinuity
    // We get rid of this by finding the critical point:
    auto criticalPoint = m_DoseMid - m_EDmid;

    // Then arbitrarily define a lower bound where the curve should be OK
    auto lowBound = criticalPoint - toUgL(23.0);
    // And a higher bound as well
    auto highBound = criticalPoint + toUgL(37.0);
    if ((m_D < criticalPoint) && (m_D > lowBound)) {
        // If the dose is in [lowerBoud, criticalPoint], use the value at the bound
        tvbio = m_F *(1.0 + m_Fmax * (lowBound - m_DoseMid) / (m_EDmid + (lowBound - m_DoseMid)));
    }
    else if ((m_D > criticalPoint) && (m_D < highBound)) {
        // If the dose in is [criticalPoint, higherBound], use the value at the bound
        tvbio = m_F *(1.0 + m_Fmax * (highBound - m_DoseMid) / (m_EDmid + (highBound - m_DoseMid)));
    }
    else {
        // Else use the initial formula
        tvbio = m_F *(1.0 + m_Fmax * (m_D - m_DoseMid) / (m_EDmid + (m_D - m_DoseMid)));
    }

    // Just a final check about the value
    if (tvbio <= 0.0) {
        tvbio = 0.5;
    }

    // TODO : Check how to deal with variability of bio here... OK, it should be on F
    auto bio = tvbio;
    auto lbpd = std::log(bio * m_D);
    auto lktr = std::log(m_ktr);
    m_cumul = lbpd + lktr - l;

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkStrictlyPositiveValue(m_Kenz, "Kenz");
    bOK &= checkStrictlyPositiveValue(m_Emax, "Emax");
    bOK &= checkStrictlyPositiveValue(m_ECmid, "ECmid");
    bOK &= checkStrictlyPositiveValue(m_EDmid, "EDmid");
    bOK &= checkStrictlyPositiveValue(m_AllmCL, "Allometric clearance");
    bOK &= checkStrictlyPositiveValue(m_DoseMid, "DoseMid");
    bOK &= checkStrictlyPositiveValue(m_NN, "NN");
    bOK &= checkStrictlyPositiveValue(m_MTT, "MTT");
    bOK &= checkCondition(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkMichaelisMentenEnzymeBolus::RkMichaelisMentenEnzymeBolus() : RkMichaelisMentenEnzyme()
{
}

std::vector<std::string> RkMichaelisMentenEnzymeBolus::getParametersId()
{
    return {"V", "Km", "Vmax", "F", "Kenz", "Emax", "ECmid", "AllmCL"};
}

bool RkMichaelisMentenEnzymeBolus::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 7, "The number of parameters should be equal to 7.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = 0.0; // _parameters.getValue(ParameterId::Ka);
    m_DoseMid = 0.0;
    m_EDmid = 0.0;
    m_NN = 0.0;
    m_MTT = 0.0;


    m_Kenz = _parameters.getValue(ParameterId::Kenz);
    m_Emax = _parameters.getValue(ParameterId::Emax);
    m_ECmid = _parameters.getValue(ParameterId::ECmid);

    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Kenz, "Kenz");
    bOK &= checkStrictlyPositiveValue(m_Emax, "Emax");
    bOK &= checkStrictlyPositiveValue(m_ECmid, "ECmid");
    bOK &= checkCondition(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

/*
RkMichaelisMentenEnzymeInfusion::RkMichaelisMentenEnzymeInfusion() : RkMichaelisMentenEnzyme()
{
}

std::vector<std::string> RkMichaelisMentenEnzymeInfusion::getParametersId()
{
    return {"V", "Km", "Vmax", "F"};
}

bool RkMichaelisMentenEnzymeInfusion::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 4, "The number of parameters should be equal to 4.")) {
        return false;
    }

    m_D = _intakeEvent.getDose() * 1000;
    m_V = _parameters.getValue(ParameterId::V);
    m_Km = _parameters.getValue(ParameterId::Km);
    m_Vmax = _parameters.getValue(ParameterId::Vmax);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Km, "The Michaelis Menten constant");
    bOK &= checkStrictlyPositiveValue(m_Vmax, "VMax");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkCondition(m_nbPoints >= 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}

*/


} // namespace Core
} // namespace Tucuxi
