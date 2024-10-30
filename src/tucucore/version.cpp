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


#include "version.h"

// The following 3 lines are here to avoid linting messages
#ifndef TUCUXI_GIT_REVISION
#define TUCUXI_GIT_REVISION "ERROR_NOREVISION" // NOLINT(cppcoreguidelines-macro-usage)
#endif                                         // TUCUXI_GIT_REVISION


namespace Tucuxi {
namespace Core {

Version::Version() = default;

std::string Version::getGitRevision()
{
    return TUCUXI_GIT_REVISION; // NOLINT(clang-diagnostic-error)
}


} // namespace Core
} // namespace Tucuxi
