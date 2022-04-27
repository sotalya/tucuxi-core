//@@lisence@@

#include <iostream>

#include "computingresult.h"

namespace Tucuxi {
namespace Core {

std::ostream& operator<<(std::ostream& _stream, const ComputingStatus& _e)
{
    return _stream << static_cast<typename std::underlying_type<ComputingStatus>::type>(_e);
}

} // namespace Core
} // namespace Tucuxi
