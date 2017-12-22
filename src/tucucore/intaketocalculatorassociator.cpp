/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "intaketocalculatorassociator.h"


namespace Tucuxi {
namespace Core {


IntakeToCalculatorAssociator::Result IntakeToCalculatorAssociator::associate(
        Tucuxi::Core::IntakeSeries& _intakes,
        const Tucuxi::Core::PkModel &_pkModel)
{
    IntakeSeries::iterator it = _intakes.begin();
    while (it != _intakes.end()) {
        std::shared_ptr<IntakeIntervalCalculator> pCalc = _pkModel.getCalculatorForRoute(it->getRoute());
        if (pCalc == nullptr) {
            return IntakeToCalculatorAssociator::Result::UnsupportedRoute;
        }
        it->setCalculator(pCalc);
        it++;
    }
    return IntakeToCalculatorAssociator::Result::Ok;
}

}
}
