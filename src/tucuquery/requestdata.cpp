#include "querydata.h"

using namespace std;
using namespace Tucuxi;

namespace Tucuxi {
namespace Query {

// DateInterval class

DateInterval::DateInterval(Common::DateTime& _start, Common::DateTime& _end)
    : m_start(_start), m_end(_end)
{}

const Common::DateTime DateInterval::getStart() const
{
    return m_start;
}

const Common::DateTime DateInterval::getEnd() const
{
    return m_end;
}

// GraphData class

GraphData::GraphData(unique_ptr<DateInterval> _pDateInterval, vector<unsigned short> _percentiles)
    : m_pDateInterval(move(_pDateInterval)), m_percentiles(move(_percentiles))
{}

const DateInterval& GraphData::getpDateInterval() const
{
    return *m_pDateInterval;
}

vector<unsigned short> GraphData::getPercentiles() const
{
    return m_percentiles;
}

// Backextrapolation class

Backextrapolation::Backextrapolation(unique_ptr<SampleData> _pSample, unique_ptr<Core::Dosage> _pDosage)
    : m_pSample(move(_pSample)), m_pDosage(move(_pDosage))
{}

const SampleData& Backextrapolation::getpSample() const
{
    return *m_pSample;
}

const Core::Dosage& Backextrapolation::getpDosage() const
{
    return *m_pDosage;
}

// RequestData class

RequestData::RequestData(string& _requestID,
        string& _drugID,
        string& _drugModelID,
        string& _requestType,
        int _nbPointsPerHour,
        unique_ptr<DateInterval> _pDateInterval,
        string& _parametersType,
        unique_ptr<GraphData> _pGraph,
        std::vector<double> &_percentiles,
        unique_ptr<Backextrapolation> _pBackextrapolation,
        std::vector<Tucuxi::Common::DateTime> _pointsInTime) :
    m_requestID(_requestID), m_drugID(_drugID), m_drugModelID(_drugModelID),
    m_requestType(_requestType),
    m_nbPointsPerHour(_nbPointsPerHour),
    m_pDateInterval(move(_pDateInterval)), m_parametersType(_parametersType),
    m_pGraph(move(_pGraph)), m_percentiles(_percentiles),
    m_pBackextrapolation(move(_pBackextrapolation)),
    m_pointsInTime(_pointsInTime)
{}

const string RequestData::getRequestID() const
{
    return m_requestID;
}

const string RequestData::getDrugID() const
{
    return m_drugID;
}

const string RequestData::getDrugModelID() const
{
    return m_drugModelID;
}

const string RequestData::getRequestType() const
{
    return m_requestType;
}

int RequestData::getNbPointsPerHour() const
{
    return m_nbPointsPerHour;
}

const DateInterval& RequestData::getpDateInterval() const
{
    return *m_pDateInterval;
}

const string RequestData::getParametersType() const
{
    return m_parametersType;
}

const GraphData& RequestData::getpGraph() const
{
    return *m_pGraph;
}

const vector<double> RequestData::getPercentiles() const
{
    return m_percentiles;
}

const Backextrapolation& RequestData::getpBackextrapolation() const
{
    return *m_pBackextrapolation;
}

const std::vector<Tucuxi::Common::DateTime>& RequestData::getPointsInTime() const
{
    return m_pointsInTime;
}

} // namespace Query
} // namespace Tucuxi
