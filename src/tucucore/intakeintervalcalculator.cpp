/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "tucucommon/loggerhelper.h"
#include "tucucommon/general.h"

#include "tucucore/intakeevent.h"
#include "tucucore/intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {



void PertinentTimesCalculatorStandard::calculateTimes(const IntakeEvent& _intakeEvent,
              int _nbPoints,
              Eigen::VectorXd& _times)
{
    if (_nbPoints == 1) {
        _times[0] = _intakeEvent.getInterval().toHours();
        return;
    }
    for(int i = 0; i < _nbPoints; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(_nbPoints - 1) * static_cast<double>(_intakeEvent.getInterval().toHours());
    }
}

void PertinentTimesCalculatorInfusion::calculateTimes(const IntakeEvent& _intakeEvent,
              int _nbPoints,
              Eigen::VectorXd& _times)
{
    double infusionTime = std::min(_intakeEvent.getInfusionTime().toHours(), _intakeEvent.getInterval().toHours());
    double interval = _intakeEvent.getInterval().toHours();

    if (_nbPoints == 2) {
        _times[0] = 0;
        _times[1] = interval;
        return;
    }

    if (_nbPoints == 1) {
        _times[0] = interval;
        return;
    }

    double postTime = interval - infusionTime;

    int nbInfus = std::min(_nbPoints, std::max(2, static_cast<int>((infusionTime / interval) * static_cast<double>(_nbPoints))));
    int nbPost = _nbPoints - nbInfus;

    for(int i = 0; i < nbInfus; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(nbInfus - 1) * infusionTime;
    }

    for(int i = 0; i < nbPost; i++) {
        _times[i + nbInfus] = infusionTime + static_cast<double>(i + 1) / static_cast<double>(nbPost) * postTime;
    }
}


IntakeIntervalCalculator::~IntakeIntervalCalculator()
= default;



bool IntakeIntervalCalculator::checkCondition(bool _isOk, const std::string& _errMsg)
{
    if (!_isOk) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error(_errMsg);
        }

    }
    return _isOk;
}

bool IntakeIntervalCalculator::checkValidValue(Value _value, const std::string &_valueName)
{
    // First check for infinity
    if (std::isinf(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is Inf.", _valueName);
        }
        return false;
    }

    // Finally check for NaN
    if (std::isnan(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is NaN.", _valueName);
        }
        return false;
    }

    return true;
}

bool IntakeIntervalCalculator::checkPositiveValue(Value _value,  const std::string& _valueName)
{
    // First check for infinity
    if (std::isinf(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is Inf.", _valueName);
        }
        return false;
    }

    // If not infinity
    // Here we use the fact that if _value is NaN, the comparison will return false
    if (_value >= 0.0) {
        return true;
    }

    // Finally check for NaN
    if (std::isnan(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is NaN.", _valueName);
        }
        return false;
    }

    // It is a negative number
    if (m_loggingErrors) {
        static Tucuxi::Common::LoggerHelper logger;
        logger.error("{} is negative", _valueName);
    }
    return false;
}

bool IntakeIntervalCalculator::checkStrictlyPositiveValue(Value _value,  const std::string& _valueName)
{
    // First check for infinity
    if (std::isinf(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is Inf.", _valueName);
        }
        return false;
    }

    // If not infinity
    // Here we use the fact that if _value is NaN, the comparison will return false
    if (_value > 0.0) {
        return true;
    }

    // Finally check for NaN
    if (std::isnan(_value)) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error("{} is NaN.", _valueName);
        }
        return false;
    }

    // It is a negative number
    if (m_loggingErrors) {
        static Tucuxi::Common::LoggerHelper logger;
        if (_value == 0) {
            logger.error("{} is equal to 0", _valueName);
        }
        else {
            logger.error("{} is negative", _valueName);
        }
    }
    return false;
}



} // namespace Core
} // namespace Tucuxi

