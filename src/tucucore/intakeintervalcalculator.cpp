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



bool IntakeIntervalCalculator::checkValue(bool _isOk, const std::string& _errMsg)
{
    if (!_isOk) {
        if (m_loggingErrors) {
            static Tucuxi::Common::LoggerHelper logger;
            logger.error(_errMsg);
        }
    }
    return _isOk;
}


} // namespace Core
} // namespace Tucuxi

