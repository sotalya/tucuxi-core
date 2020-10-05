
#include "computingresult.h"

#include <iostream>

namespace Tucuxi {
namespace Core {

std::ostream& operator<<(std::ostream& stream, const ComputingStatus& e)
{
    return stream << static_cast<typename std::underlying_type<ComputingStatus>::type>(e);
}

} // namespace Core
} // namespace Tucuxi
