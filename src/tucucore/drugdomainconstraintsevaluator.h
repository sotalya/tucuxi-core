/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H
#define TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H

#include "tucucommon/datetime.h"

#include "tucucore/covariateevent.h"
#include "tucucore/drugmodel/drugmodel.h"

namespace Tucuxi {
namespace Core {

class DrugModelDomain;
class Constraint;
class DrugTreatment;

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
    enum class Result
    {

        /// The patient covariates are incompatible with the drug model : hard constraints are not met
        Incompatible,

        /// The patient covariates are not totally compatible: soft constraints are not met
        PartiallyCompatible,

        /// The patient covariates are fully compatible with the drug model
        Compatible,

        /// The extraction of covariates ended up with an issue, because of bad covariate list
        CovariateExtractionError,

        /// A computation error occured
        ComputationError
    };

    typedef struct
    {
        Constraint* m_constraint;
        Result m_result;
    } EvaluationResult;

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
    /// \param _results An empty vector that will contain the evaluation of each constraint
    /// \return The result of computation
    ///
    /// This function returns:
    /// - Result::Incompatible if at least a hard constraint is not met.
    /// - Result::PartiallyCompatible if all hard constraints are met and at least a soft constraint is not.
    /// - Result::Compatible if all constraints are met.
    /// - Result::ComputationError if there was a computation error
    ///
    /// The covariate series should typically be the series extracted by a CovariateExtractor object.
    /// The evaluation is made throughout the time span from _start to _end, so if covariates change during
    /// this time frame there will be various evaluations of the adequation of covariates with respect to
    /// the constraints of the drug model domain.
    ///
    Result evaluate(
            const DrugModelDomain& _drugDomain,
            const CovariateSeries& _covariates,
            const Common::DateTime& _start,
            const Common::DateTime& _end,
            std::vector<EvaluationResult>& _results);


    ///
    /// \brief evaluate the adequation of covariates with a drug model domain
    /// \param _drugDomain A drug model domain
    /// \param _drugTreatment A drug treatment meant to be used for computing requests
    /// \param _start The starting time of the evaluation range
    /// \param _start The ending time of the evaluation range
    /// \param _results An empty vector that will contain the evaluation of each constraint
    /// \return A global result of the evaluation.
    ///
    /// This function returns:
    /// - Result::Incompatible if at least a hard constraint is not met.
    /// - Result::PartiallyCompatible if all hard constraints are met and at least a soft constraint is not.
    /// - Result::Compatible if all constraints are met.
    /// - Result::ComputationError if there was a computation error
    ///
    /// Each constraint of the DrugModelDomain is evaluated individually, on the time range specified by [_start, _end].
    /// Each evaluation ends up being an EvaluationResult
    /// appended to the _results vector. This vector contains tuples of pointers to the constraint and evaluation result.
    /// It allows to generate a report based on the extra information found in the constraint, for instance by getting a
    /// constraint description related to a specific language.
    ///
    /// The evaluation of a single constraint is the following:
    ///
    /// For a SOFT constraint:
    /// - If the necessary covariates do not exist in the drug treatment, then it is Compatible
    /// - If the necessary covariates do exist and end up with a non-respect of the constraint, then it is PartiallyCompatible
    /// - If the necessary covariates do exist and end up with a respect of the constraint, then it is Compatible
    ///
    /// For a HARD constraint:
    /// - If the necessary covariates do not exist in the drug treatment, then it is Incompatible
    /// - If the necessary covariates do exist and end up with a non-respect of the constraint, then it is Incompatible
    /// - If the necessary covariates do exist and end up with a respect of the constraint, then it is Compatible
    ///
    /// This function is meant to be used for an expert system, to decide if a specific DrugModel can be used
    /// for a specific patient or not. It is not meant to by used within a ComputingComponent, as it is not the
    /// responsibility of a ComputingComponent to take this decision.
    ///
    Result evaluate(
            const DrugModel& _drugModel,
            const DrugTreatment& _drugTreatment,
            const DateTime& _start,
            const DateTime& _end,
            std::vector<EvaluationResult>& _results);
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DRUGDOMAINCONSTRAINTSEVALUATOR_H
