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
        bool m_ok;
        std::string m_errorMessage;
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

    CheckerResult_t checkParameters(const DrugModel *_drugModel, const PkModelCollection *_pkCollection);

    CheckerResult_t checkOperations(const DrugModel *_drugModel);

    ///
    /// \brief Checks if the half life and its multiplier are OK
    /// \param _drugModel The drug model to be validated
    /// \return
    ///
    /// This method checks whether the half life and the associated multiplier offer a sufficient
    /// duration to reach a kind of steady state. The check is done by creating a dosage corresponding
    /// to the default formulation and route (dose + interval). Then, we compute two cycles at a
    /// time being halfLife * multiplier from the first dose. We compare the first concentration of
    /// the two cycles, and if the difference is smaller than 0.5%, then we consider the values
    /// to be relevant.
    ///
    CheckerResult_t checkHalfLife(const DrugModel *_drugModel);

    void getAllOperations(const DrugModel *_drugModel, std::vector<Operation *> &_operations);
};

} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_DRUGMODELCHECKER_H
