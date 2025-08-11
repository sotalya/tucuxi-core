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


#include "targetdefinition.h"


namespace Tucuxi {
namespace Core {

std::string toString(TargetType _type)
{
    static std::map<TargetType, std::string> map = {
            {TargetType::UnknownTarget, "unknown"},
            {TargetType::Residual, "residual"},
            {TargetType::Peak, "peak"},
            {TargetType::Mean, "mean"},
            {TargetType::Auc, "auc"},
            {TargetType::Auc24, "auc24"},
            {TargetType::CumulativeAuc, "CumulativeAuc"},
            {TargetType::AucOverMic, "aucOverMic"},
            {TargetType::Auc24OverMic, "auc24OverMic"},
            {TargetType::TimeOverMic, "timeOverMic"},
            {TargetType::AucDividedByMic, "aucDividedByMic"},
            {TargetType::Auc24DividedByMic, "auc24DividedByMic"},
            {TargetType::PeakDividedByMic, "peakDividedByMic"},
            {TargetType::ResidualDividedByMic, "residualDividedByMic"},
            {TargetType::FractionTimeOverMic, "fractionTimeOverMic"}};
    return map.at(_type);
}


} // namespace Core
} // namespace Tucuxi
