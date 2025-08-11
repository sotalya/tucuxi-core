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


#ifndef TUCUXI_QUERY_QUERYSTATUS_H
#define TUCUXI_QUERY_QUERYSTATUS_H


namespace Tucuxi {
namespace Query {

enum class [[nodiscard]] QueryStatus{/// Everything fine
                                     Ok = 0,
                                     /// Some requests are wrong
                                     PartiallyOk,
                                     /// None of the requests could be calculated correctly
                                     Error,
                                     /// Error during xml query import
                                     ImportError,
                                     /// Xml file or string with bad format (not imported yet)
                                     BadFormat,
                                     /// Undefined (should not be observed when exporting a query)
                                     Undefined};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_QUERYSTATUS_H
