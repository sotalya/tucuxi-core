//@@license@@

#include <utility>

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
    : Core::Sample(std::move(_date), std::move(_analyteId), _value, std::move(_unit), _weight),
      m_sampleID(std::move(_id))
{
}

std::string FullSample::getSampleId() const
{
    return m_sampleID;
}

} // namespace Query
} // namespace Tucuxi
