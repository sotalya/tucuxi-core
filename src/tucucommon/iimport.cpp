//@@license@@

#include "iimport.h"

namespace Tucuxi {
namespace Common {

std::ostream& operator<<(std::ostream& _stream, const IImport::Status& _e)
{
    return _stream << static_cast<typename std::underlying_type<IImport::Status>::type>(_e);
}


} // namespace Common
} // namespace Tucuxi
