/*
* Copyright (C) 2017 Tucuxi SA
*/

#include "intaketocalculatorassociator.h"


namespace Tucuxi {
namespace Core {

IntakeToCalculatorAssociator::IntakeToCalculatorAssociator()
{

}


IntakeToCalculatorAssociator::Result IntakeToCalculatorAssociator::associate(
        Tucuxi::Core::IntakeSeries& _intakes,
        const Tucuxi::Core::PkModel &_pkModel)
{
    // Iterate through the intake series, and get the calculator from pkModel.
    // Be careful, if routes are the same, we have to reuse the previous calculator

}

}
}
