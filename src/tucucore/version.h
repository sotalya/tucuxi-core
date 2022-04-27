//@@lisence@@

#ifndef VERSION_H
#define VERSION_H

#include <string>

namespace Tucuxi {
namespace Core {

class Version
{
public:
    Version();

    static std::string getGitRevision();
};


} // namespace Core
} // namespace Tucuxi


#endif // VERSION_H
