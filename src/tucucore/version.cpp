#include "version.h"


namespace Tucuxi {
namespace Core {

Version::Version()
{

}

std::string Version::getGitRevision()
{
    return TUCUXI_GIT_REVISION; // NOLINT(clang-diagnostic-error)
}


} // namespace Core
} // namespace Tucuxi
