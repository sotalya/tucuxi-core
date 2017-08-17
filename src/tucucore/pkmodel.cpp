/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "pkmodel.h"
#include "intakeintervalcalculator.h"
#include "onecompartmentbolus.h"

namespace Tucuxi {
namespace Core {

PkModel::PkModel()
{

}


IntakeIntervalCalculator *PkModel::getCalculatorForRoute(RouteOfAdministration _route) const
{
    auto search = m_calculatorCreators.find(_route);
    if(search != m_calculatorCreators.end()) {
        return search->second->create();
    }
    else {
        return nullptr;
    }
}

}
}
