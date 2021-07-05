#include "iimport.h"

namespace Tucuxi {
namespace Common {

std::ostream& operator<<(std::ostream& stream, const IImport::Status& e)
{
    return stream << static_cast<typename std::underlying_type<IImport::Status>::type>(e);
}


} // namespace Common
} // namespace Tucuxi
