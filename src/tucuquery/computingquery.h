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


#ifndef QUERY_H
#define QUERY_H

#include <memory>
#include <string>
#include <vector>

namespace Tucuxi {

namespace Core {
class ComputingRequest;
} // namespace Core

namespace Query {



class ComputingQuery
{
public:
    ComputingQuery(std::string _queryId);

    void addComputingRequest(std::unique_ptr<Tucuxi::Core::ComputingRequest> _computingRequest);

    std::string m_queryId;

    std::vector<std::unique_ptr<Tucuxi::Core::ComputingRequest> > m_computingRequests;
};


} // namespace Query
} // namespace Tucuxi


#endif // QUERY_H
