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


#ifndef QUERYCOMPUTER_H
#define QUERYCOMPUTER_H

#include "tucuquery/computingquery.h"
#include "tucuquery/computingqueryresponse.h"

namespace Tucuxi {
namespace Query {


class QueryComputer
{
public:
    QueryComputer();

    ///
    /// \brief computes a full query, and populates the responses
    /// \param _query The query object, embedded all information
    /// \param _response The query response
    /// The list of responses embedded in the query response correspond to the
    /// ComputingRequest embedded in _query
    ///
    void compute(ComputingQuery& _query, ComputingQueryResponse& _response, const std::string& _tqfOutputFileName = "");

    ///
    /// \brief computes a full query, and populates the responses
    /// \param _queryString The XML string representing the query
    /// \param _response The query response
    /// The list of responses embedded in the query response correspond to the
    /// ComputingRequest embedded in _queryString
    ///
    void compute(
            const std::string& _queryString,
            ComputingQueryResponse& _response,
            const std::string& _tqfOutputFileName = "");
};


} // namespace Query
} // namespace Tucuxi


#endif // QUERYCOMPUTER_H
