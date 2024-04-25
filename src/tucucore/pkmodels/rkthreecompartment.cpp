//@@license@@

#include "rkthreecompartment.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {


RkThreeCompartment::RkThreeCompartment()
    : IntakeIntervalCalculatorRK4Base<4, RkThreeCompartment>(std::make_unique<PertinentTimesCalculatorStandard>())
{
    m_Tinf = 0.0;
    m_Tlag = 0.0;
}


RkThreeCompartmentExtraMicro::RkThreeCompartmentExtraMicro() : RkThreeCompartment()
{
    m_delivered = true;
    m_isWithLag = false;
}

std::vector<std::string> RkThreeCompartmentExtraMicro::getParametersId()
{
    return {"V1", "Ke", "K12", "K21", "K13", "K31", "F", "Ka"};
}

bool RkThreeCompartmentExtraMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 8, "The number of parameters should be equal to 8.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkThreeCompartmentExtraLagMicro::RkThreeCompartmentExtraLagMicro() : RkThreeCompartment()
{
    m_delivered = false;
    m_isWithLag = true;
}

std::vector<std::string> RkThreeCompartmentExtraLagMicro::getParametersId()
{
    return {"V1", "Ke", "K12", "K21", "K13", "K31", "F", "Ka", "Tlag"};
}

bool RkThreeCompartmentExtraLagMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 9, "The number of parameters should be equal to 9.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkThreeCompartmentBolusMicro::RkThreeCompartmentBolusMicro() : RkThreeCompartment()
{
    m_delivered = true;
    m_isWithLag = false;
    m_Ka = 0.0;
    m_Tlag = 0.0;
    m_F = 0.0;
}

std::vector<std::string> RkThreeCompartmentBolusMicro::getParametersId()
{
    return {"V1", "Ke", "K12", "K21", "K13", "K31"};
}

bool RkThreeCompartmentBolusMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getOptionalValue(ParameterId::F);
    m_Ka = _parameters.getOptionalValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkThreeCompartmentInfusionMicro::RkThreeCompartmentInfusionMicro() : RkThreeCompartment()
{
    m_delivered = true;
    m_isInfusion = true;
    m_isWithLag = false;
}

std::vector<std::string> RkThreeCompartmentInfusionMicro::getParametersId()
{
    return {"V1", "Ke", "K12", "K21", "K13", "K31"};
}

bool RkThreeCompartmentInfusionMicro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_nonDifferentiableTime = m_Tinf;

    const double eps = 0.001;
    m_TinfLow = m_Tinf - eps;
    m_TinfHigh = m_Tinf + eps;

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    m_Ke = _parameters.getValue(ParameterId::Ke);
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_K12 = _parameters.getValue(ParameterId::K12);
    m_K21 = _parameters.getValue(ParameterId::K21);
    m_K13 = _parameters.getValue(ParameterId::K13);
    m_K31 = _parameters.getValue(ParameterId::K31);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}






















RkThreeCompartmentExtraMacro::RkThreeCompartmentExtraMacro() = default;



std::vector<std::string> RkThreeCompartmentExtraMacro::getParametersId()
{
    return {"V1", "V2", "V3", "CL", "Q2", "Q3", "F", "Ka"};
}

bool RkThreeCompartmentExtraMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 8, "The number of parameters should be equal to 8.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q2 = _parameters.getValue(ParameterId::Q2);
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    auto v3 = _parameters.getValue(ParameterId::V3);
    auto q3 = _parameters.getValue(ParameterId::Q3);
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkThreeCompartmentExtraLagMacro::RkThreeCompartmentExtraLagMacro() = default;

std::vector<std::string> RkThreeCompartmentExtraLagMacro::getParametersId()
{
    return {"V1", "V2", "V3", "CL", "Q2", "Q3", "F", "Ka", "Tlag"};
}

bool RkThreeCompartmentExtraLagMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 9, "The number of parameters should be equal to 9.")) {
        return false;
    }

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q2 = _parameters.getValue(ParameterId::Q2);
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    auto v3 = _parameters.getValue(ParameterId::V3);
    auto q3 = _parameters.getValue(ParameterId::Q3);
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_F = _parameters.getValue(ParameterId::F);
    m_Ka = _parameters.getValue(ParameterId::Ka);
    m_Tlag = _parameters.getValue(ParameterId::Tlag);
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkStrictlyPositiveValue(m_F, "The bioavailability");
    bOK &= checkStrictlyPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}


RkThreeCompartmentBolusMacro::RkThreeCompartmentBolusMacro() = default;

std::vector<std::string> RkThreeCompartmentBolusMacro::getParametersId()
{
    return {"V1", "V2", "V3", "CL", "Q2", "Q3"};
}

bool RkThreeCompartmentBolusMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }


    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q2 = _parameters.getValue(ParameterId::Q2);
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    auto v3 = _parameters.getValue(ParameterId::V3);
    auto q3 = _parameters.getValue(ParameterId::Q3);
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}
















RkThreeCompartmentInfusionMacro::RkThreeCompartmentInfusionMacro() = default;

std::vector<std::string> RkThreeCompartmentInfusionMacro::getParametersId()
{
    return {"V1", "V2", "V3", "CL", "Q2", "Q3"};
}

bool RkThreeCompartmentInfusionMacro::checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters)
{
    if (!checkCondition(_parameters.size() >= 6, "The number of parameters should be equal to 6.")) {
        return false;
    }

    m_Tinf = (_intakeEvent.getInfusionTime()).toHours();
    m_nonDifferentiableTime = m_Tinf;

    const double eps = 0.001;
    m_TinfLow = m_Tinf - eps;
    m_TinfHigh = m_Tinf + eps;

    m_D = _intakeEvent.getDose();
    m_V1 = _parameters.getValue(ParameterId::V1);
    auto cl = _parameters.getValue(ParameterId::CL);
    m_Ke = cl / m_V1;
    auto v2 = _parameters.getValue(ParameterId::V2);
    auto q2 = _parameters.getValue(ParameterId::Q2);
    m_K12 = q2 / m_V1;
    m_K21 = q2 / v2;
    auto v3 = _parameters.getValue(ParameterId::V3);
    auto q3 = _parameters.getValue(ParameterId::Q3);
    m_K13 = q3 / m_V1;
    m_K31 = q3 / v3;
    m_nbPoints = _intakeEvent.getNbPoints();
    m_Int = (_intakeEvent.getInterval()).toHours();

    // check the inputs
    bool bOK = true;
    bOK &= checkPositiveValue(m_D, "The dose");
    bOK &= checkStrictlyPositiveValue(m_V1, "The volume");
    bOK &= checkPositiveValue(m_F, "The bioavailability");
    bOK &= checkPositiveValue(m_Ka, "The absorption rate");
    bOK &= checkPositiveValue(m_Tlag, "The lag time");
    bOK &= checkPositiveValue(m_K12, "K12");
    bOK &= checkPositiveValue(m_K21, "K21");
    bOK &= checkPositiveValue(m_K13, "K13");
    bOK &= checkPositiveValue(m_K31, "K31");
    bOK &= checkPositiveValue(m_Ke, "Ke");
    bOK &= checkCondition(m_nbPoints > 0, "The number of points is zero or negative.");
    bOK &= checkCondition(m_Int > 0, "The interval time is negative.");

    return bOK;
}







} // namespace Core
} // namespace Tucuxi
