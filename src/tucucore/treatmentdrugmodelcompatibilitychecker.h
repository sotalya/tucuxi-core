#ifndef TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H
#define TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H

namespace Tucuxi {
namespace Core {

class DrugModel;
class DrugTreatment;

///
/// \brief The TreatmentDrugModelCompatibilityChecker class
/// The aim of this class is to offer a compatibility check function
/// between a drug treatment and a drug model.
/// It is used by the ComputingComponent as a first check
///
class TreatmentDrugModelCompatibilityChecker
{
public:
    ///
    /// \brief Empty constructor
    ///
    TreatmentDrugModelCompatibilityChecker();

    ///
    /// \brief Checks compatibility between a drug treatment and a drug model
    /// \param _drugTreatment The drug treatment
    /// \param _drugModel The drug model
    /// \return true if the model is compatible with the treatment, false else
    ///
    /// First if one of the input parameters is nullptr, the function returns false.
    /// Second, it checks that each formulation and route of the drug treatment
    /// is available in the drug model. If this is not the case, false is returned.
    ///
    bool checkCompatibility(const DrugTreatment *_drugTreatment,
                            const DrugModel *_drugModel);

};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_TREATMENTDRUGMODELCOMPATIBILITYCHECKER_H
