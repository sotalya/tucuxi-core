#include "querydata.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

// DateInterval class

DateInterval::DateInterval(Common::DateTime& _start, Common::DateTime& _end) : m_start(_start), m_end(_end) {}

Common::DateTime DateInterval::getStart() const
{
    return m_start;
}

Tucuxi::Common::DateTime DateInterval::getEnd() const
{
    return m_end;
}

// GraphData class

GraphData::GraphData(unique_ptr<DateInterval> _pDateInterval, vector<unsigned short> _percentiles)
    : m_pDateInterval(move(_pDateInterval)), m_percentiles(move(_percentiles))
{
}

const DateInterval& GraphData::getpDateInterval() const
{
    return *m_pDateInterval;
}

vector<unsigned short> GraphData::getPercentiles() const
{
    return m_percentiles;
}

//// Backextrapolation class

//Backextrapolation::Backextrapolation(unique_ptr<SampleData> _pSample, unique_ptr<Core::Dosage> _pDosage)
//    : m_pSample(move(_pSample)), m_pDosage(move(_pDosage))
//{}

//const SampleData& Backextrapolation::getpSample() const
//{
//    return *m_pSample;
//}

//const Core::Dosage& Backextrapolation::getpDosage() const
//{
//    return *m_pDosage;
//}

// RequestData class

RequestData::RequestData(
        string& _requestID, string& _drugID, string& _drugModelID, unique_ptr<Core::ComputingTrait> _pComputingTrait)
    : m_pComputingTrait(move(_pComputingTrait)), m_requestID(_requestID), m_drugID(_drugID), m_drugModelID(_drugModelID)
{
}

string RequestData::getRequestID() const
{
    return m_requestID;
}

std::string RequestData::getDrugID() const
{
    return m_drugID;
}

string RequestData::getDrugModelID() const
{
    return m_drugModelID;
}

Tucuxi::Core::ComputingTrait& RequestData::getpComputingTrait() const
{
    return *m_pComputingTrait;
}

} // namespace Query
} // namespace Tucuxi
