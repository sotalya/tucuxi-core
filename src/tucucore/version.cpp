#include "version.h"

// The following 3 lines are here to avoid linting messages
#ifndef TUCUXI_GIT_REVISION
    #define TUCUXI_GIT_REVISION "ERROR_NOREVISION"
#endif // TUCUXI_GIT_REVISION


namespace Tucuxi {
namespace Core {

Version::Version()
= default;

std::string Version::getGitRevision()
{
    return TUCUXI_GIT_REVISION; // NOLINT(clang-diagnostic-error)
}


} // namespace Core
} // namespace Tucuxi
