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


#include "intakeevent.h"



namespace Tucuxi {
namespace Core {


void cloneIntakeSeries(const std::vector<IntakeEvent>& _input, std::vector<IntakeEvent>& _output)
{
    for (const auto& intake : _input) {
        IntakeEvent newIntakeEvent = intake;
        if (intake.getCalculator() != nullptr) {
            newIntakeEvent.setCalculator(intake.getCalculator()->getLightClone());
        }
        _output.push_back(newIntakeEvent);
    }
}

void selectRecordedIntakes(
        IntakeSeries& _selectionSeries,
        const IntakeSeries& _intakeSeries,
        const DateTime& _recordFrom,
        const DateTime& _recordTo)
{
    for (const auto& intake : _intakeSeries) {
        DateTime eventTime = intake.getEventTime();
        if ((eventTime + intake.getInterval() > _recordFrom) && (eventTime < _recordTo)) {
            _selectionSeries.push_back(IntakeEvent(intake));
        }
    }
}


} // namespace Core
} // namespace Tucuxi
