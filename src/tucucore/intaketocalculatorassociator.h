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


#ifndef TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H
#define TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/dosage.h"
#include "tucucore/pkmodel.h"

namespace Tucuxi {
namespace Core {


class IntakeToCalculatorAssociator
{
public:
    ///
    /// \brief associate Associates the calculators to the intakes
    /// \param _intakes List of intakes in time
    /// \param _pkModel The PkModel responsible to create the IntakeIntervalCalculators
    /// \return Ok if everything went well, UnsupportedRoute in case there is no calculator for the specified route
    /// This function should note create duplicates of IntakeIntervalCalculators. If the same routes are used, then
    /// a single calculator should be created
    static ComputingStatus associate(Tucuxi::Core::IntakeSeries& _intakes, const Tucuxi::Core::PkModel& _pkModel);
};

} // namespace Core
} // namespace Tucuxi
#endif // TUCUXI_CORE_INTAKETOCALCULATORASSOCIATOR_H
