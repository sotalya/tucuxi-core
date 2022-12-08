//@@license@@

#include "fullsample.h"

namespace Tucuxi {
namespace Query {


FullSample::FullSample(
        std::string _id,
        Common::DateTime _date,
        Core::AnalyteId _analyteId,
        Core::Value _value,
        Common::TucuUnit _unit,
        Core::Value _weight)
    : Core::Sample(_date, _analyteId, _value, _unit, _weight), m_sampleID(_id)
{
}

std::string FullSample::getSampleId() const
{
    return m_sampleID;
}

} // namespace Query
} // namespace Tucuxi
