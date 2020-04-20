#ifndef TUCUXI_QUERY_REQUESTDATA_H
#define TUCUXI_QUERY_REQUESTDATA_H

#include <string>
#include <vector>
#include <memory>
#include "tucucommon/datetime.h"
#include "tucucore/dosage.h"
#include "parametersdata.h"
#include "tucucore/computingservice/computingtrait.h"

namespace Tucuxi {
namespace Query {

class DateInterval
{
public:
    //Constructors
    DateInterval() = delete;

    DateInterval(
        Tucuxi::Common::DateTime& _start,
        Tucuxi::Common::DateTime& _end
    );

    DateInterval(DateInterval& _other) = delete;

    // Getters
    const Tucuxi::Common::DateTime getStart() const;
    const Tucuxi::Common::DateTime getEnd() const;

protected:
    const Tucuxi::Common::DateTime m_start;
    const Tucuxi::Common::DateTime m_end;
};

class GraphData
{
public:
    // Constructors
    GraphData() = delete;

    GraphData(
        std::unique_ptr<DateInterval> _pDateInterval,
        std::vector<unsigned short> _percentiles
    );

    GraphData(GraphData& _other) = delete;

    // Getters
    const DateInterval& getpDateInterval() const;
    std::vector<unsigned short> getPercentiles() const;

protected:
    std::unique_ptr<DateInterval> m_pDateInterval;
    std::vector<unsigned short> m_percentiles;
};

class Backextrapolation
{
public:
    // Constructors
    Backextrapolation() = delete;

    Backextrapolation(
        std::unique_ptr<SampleData> _pSample,
        std::unique_ptr<Tucuxi::Core::Dosage> _pDosage
    );

    Backextrapolation(Backextrapolation& _other) = delete;

    // Getters
    const SampleData& getpSample() const;
    const Tucuxi::Core::Dosage& getpDosage() const;

protected:
    std::unique_ptr<SampleData> m_pSample;
    std::unique_ptr<Tucuxi::Core::Dosage> m_pDosage;
};

class RequestData
{
public:
    // Constructors
    RequestData() = delete;

    RequestData(
        std::string& _requestID,
        std::string& _drugID,
        std::string& _drugModelID,
        std::unique_ptr<Core::ComputingTrait> _pComputingTrait
    );

    RequestData(RequestData& _other) = delete;

    // Getters
    const std::string getRequestID() const;
    const std::string getDrugID() const;
    const std::string getDrugModelID() const;

protected:
    const std::string m_requestID;
    const std::string m_drugID;
    const std::string m_drugModelID;
    std::unique_ptr<Core::ComputingTrait> m_pComputingTrait;
};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_REQUESTDATA_H
