/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "tucucore/intakeintervalcalculator.h"

#include "tucucommon/general.h"
#include "tucucommon/loggerhelper.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {



void PertinentTimesCalculatorStandard::calculateTimes(
        const IntakeEvent& _intakeEvent, Eigen::Index _nbPoints, Eigen::VectorXd& _times)
{
    if (_nbPoints == 1) {
        _times[0] = _intakeEvent.getInterval().toHours();
        return;
    }
    for (Eigen::Index i = 0; i < _nbPoints; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(_nbPoints - 1)
                    * static_cast<double>(_intakeEvent.getInterval().toHours());
    }
}

void PertinentTimesCalculatorInfusion::calculateTimes(
        const IntakeEvent& _intakeEvent, Eigen::Index _nbPoints, Eigen::VectorXd& _times)
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

    Eigen::Index nbInfus = std::min(
            _nbPoints,
            std::max(
                    Eigen::Index{2},
                    static_cast<Eigen::Index>((infusionTime / interval) * static_cast<double>(_nbPoints))));
    Eigen::Index nbPost = _nbPoints - nbInfus;

    for (Eigen::Index i = 0; i < nbInfus; i++) {
        _times[i] = static_cast<double>(i) / static_cast<double>(nbInfus - 1) * infusionTime;
    }

    for (Eigen::Index i = 0; i < nbPost; i++) {
        _times[i + nbInfus] = infusionTime + static_cast<double>(i + 1) / static_cast<double>(nbPost) * postTime;
    }
}


IntakeIntervalCalculator::~IntakeIntervalCalculator() = default;

unsigned int IntakeIntervalCalculator::getNbAnalytes() const
{
    return m_nbAnalytes;
}

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

bool IntakeIntervalCalculator::checkValidValue(Value _value, const std::string& _valueName)
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

bool IntakeIntervalCalculator::checkPositiveValue(Value _value, const std::string& _valueName)
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

bool IntakeIntervalCalculator::checkStrictlyPositiveValue(Value _value, const std::string& _valueName)
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
