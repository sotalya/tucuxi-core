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


#ifndef TUCUXI_QUERY_REQUESTDATA_H
#define TUCUXI_QUERY_REQUESTDATA_H

#include <memory>
#include <string>
#include <vector>

#include "tucucommon/datetime.h"

#include "tucucore/computingservice/computingtrait.h"
#include "tucucore/dosage.h"

#include "parametersdata.h"

namespace Tucuxi {
namespace Query {

class DateInterval
{
public:
    //Constructors
    DateInterval() = delete;

    DateInterval(Tucuxi::Common::DateTime& _start, Tucuxi::Common::DateTime& _end);

    DateInterval(DateInterval& _other) = delete;

    // Getters
    Common::DateTime getStart() const;
    Tucuxi::Common::DateTime getEnd() const;

protected:
    const Tucuxi::Common::DateTime m_start;
    const Tucuxi::Common::DateTime m_end;
};

class GraphData
{
public:
    // Constructors
    GraphData() = delete;

    GraphData(std::unique_ptr<DateInterval> _pDateInterval, std::vector<unsigned short> _percentiles);

    GraphData(GraphData& _other) = delete;

    // Getters
    const DateInterval& getpDateInterval() const;
    std::vector<unsigned short> getPercentiles() const;

protected:
    std::unique_ptr<DateInterval> m_pDateInterval;
    std::vector<unsigned short> m_percentiles;
};

//class Backextrapolation
//{
//public:
//    // Constructors
//    Backextrapolation() = delete;

//    Backextrapolation(
//        std::unique_ptr<SampleData> _pSample,
//        std::unique_ptr<Tucuxi::Core::Dosage> _pDosage
//    );

//    Backextrapolation(Backextrapolation& _other) = delete;

//    // Getters
//    const SampleData& getpSample() const;
//    const Tucuxi::Core::Dosage& getpDosage() const;

//protected:
//    std::unique_ptr<SampleData> m_pSample;
//    std::unique_ptr<Tucuxi::Core::Dosage> m_pDosage;
//};

class RequestData
{
public:
    // Constructors
    RequestData() = delete;

    RequestData(
            std::string& _requestID,
            std::string& _drugID,
            std::string& _drugModelID,
            std::unique_ptr<Core::ComputingTrait> _pComputingTrait);

    RequestData(RequestData& _other) = delete;

    // Getters
    std::string getRequestID() const;
    std::string getDrugID() const;
    std::string getDrugModelID() const;
    Tucuxi::Core::ComputingTrait& getpComputingTrait() const;
    std::unique_ptr<Core::ComputingTrait> m_pComputingTrait;

protected:
    const std::string m_requestID;
    const std::string m_drugID;
    const std::string m_drugModelID;
};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_REQUESTDATA_H
