#ifndef TUCUXI_CORE_DRUGFILEVALIDATOR_H
#define TUCUXI_CORE_DRUGFILEVALIDATOR_H

#include <string>

namespace Tucuxi {
namespace Core {


class DrugFileValidator
{
public:
    DrugFileValidator();

    bool validate(std::string _drugFileName, std::string _testFileName);

};


} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGFILEVALIDATOR_H
