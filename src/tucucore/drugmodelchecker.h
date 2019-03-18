#ifndef TUCUXI_CORE_DRUGMODELCHECKER_H
#define TUCUXI_CORE_DRUGMODELCHECKER_H

#include <string>
#include <vector>

namespace Tucuxi {
namespace Core {

class DrugModel;
class PkModelCollection;
class PkModel;
class Operation;

class DrugModelChecker
{
public:

    typedef struct {
        bool ok;
        std::string errorMessage;
    } CheckerResult_t;

    DrugModelChecker();

    CheckerResult_t checkDrugModel(const DrugModel *_drugModel, const PkModelCollection *_pkCollection);

protected:

    ///
    /// \brief checkAnalytes
    /// \param _drugModel
    /// \return
    ///
    /// Each active moiety has a list of analyteIds.
    ///
    /// 1. Check that there is no overlap amongst moieties
    ///
    /// Each analyteGroup has a list of analytes.
    /// 2. Check that there is no overlap amongst the analyte groups
    /// 3. Check that there is an exact match between the moieties and analyte groups
    ///
    /// The valid doses have an analyte conversion.
    /// 4. Check that there is an exact match between the conversions and the analyte groups
    ///    ( no need to check with the active moieties, as rule 3 validates that)
    ///
    CheckerResult_t checkAnalytes(const DrugModel *_drugModel);

    CheckerResult_t checkFormulaInputs(const DrugModel *_drugModel);

    CheckerResult_t checkParameters(const DrugModel *_drugModel, const PkModel *_pkModel);

    void getAllOperations(const DrugModel *_drugModel, std::vector<Operation *> &_operations);
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_DRUGMODELCHECKER_H
