#ifndef TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H
#define TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H

#include "tucucommon/datetime.h"

#include "tucucore/covariateevent.h"

namespace Tucuxi {
namespace Core {

class DrugModelDomain;

///
/// \brief The DrugDomainConstraintsEvaluator class
/// This class aims at checking the compatibility of a patient's covariates with a specific
/// drug model. It offers a single method responsible to do this check.
///
class DrugDomainConstraintsEvaluator
{
public:


    ///
    /// \brief The Result enum returned by the evaluate function
    ///
    enum class Result {

        /// The patient covariates are incompatible with the drug model : hard constraints are not met
        Incompatible,

        /// The patient covariates are not totally compatible: soft constraints are not met
        PartiallyCompatible,

        /// The patient covariates are fully compatible with the drug model
        Compatible,

        /// A computation error occured
        ComputationError
    };

    ///
    /// \brief DrugDomainConstraintsEvaluator empty constructor
    ///
    DrugDomainConstraintsEvaluator();

    ///
    /// \brief evaluate the adequation of covariates with a drug model domain
    /// \param _drugDomain A drug model domain
    /// \param _covariates A series of covariates
    /// \param _start A starting time from which the covariates are evaluated
    /// \param _end An ending time for the evaluation of covariates
    /// \return The result of computation
    /// This function returns:
    /// - Result::Incompatible if at least a hard constraint is not met.
    /// - Result::PartiallyCompatible if all hard constraints are met and at least a soft constraint is not.
    /// - Result::Compatible if all constraints are met.
    /// - Result::ComputationError if there was a computation error
    ///
    /// The covariate series should typically the series extracted by a CovariateExtractor object.
    /// The evaluation is made throughout the time span from _start to _end, so if covariates change during
    /// this time frame there will be various evaluations of the adequation of covariates with respect to
    /// the constraints of the drug model domain.
    ///
    Result evaluate(const DrugModelDomain& _drugDomain, const CovariateSeries& _covariates, const Common::DateTime &_start, const Common::DateTime &_end);


};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H
