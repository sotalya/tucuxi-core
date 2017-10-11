/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_DEFINITIONS_H
#define TUCUXI_CORE_DEFINITIONS_H

/// \defgroup TucuCore Tucuxi core library
/// \brief Tucuxi core classes for the computation of predictions of drug concentrations
/// This module defines all classes needed for:
///    - computing predicitions of drug concentrations
///    - computing percentiles for the above predictions
///    - computing suggestions for the adaptation of treatments
///    - modeling relevant data for above computations

#include <vector>
#include <map>

#undef min  // Prevent problems with date.h
#undef max  // Prevent problems with date.h
#include "date/iso_week.h"

#include "Eigen/Dense"

#include "tucucommon/basetypes.h"
#include "tucucommon/datetime.h"

namespace Tucuxi {
namespace Core {


/// \ingroup TucuCore
/// \brief The numeric type that will be used throughout the library (use this to set the precision for all number values)
typedef double Value;

/// \ingroup TucuCore
/// \brief The type used to represent an absolute date and time
typedef time_t Date;

/// \ingroup TucuCore
/// \brief The type used to represent an absolute time in ms
typedef double Time;

/// \ingroup TucuCore
/// \brief The type used to represent a dose in mg
typedef Value Dose;

/// \ingroup TucuCore
/// \brief The type used to represent a concentration in mg
typedef Value Concentration;

/// \brief The type used to represent a serie of times exrpressed as offsets in millisecond to the start of a cycle
 typedef std::vector<double> TimeOffsets;

/// \ingroup TucuCore
/// \brief The type used to represent a serie of concentrations
typedef std::vector<Concentration> Concentrations;

/// \ingroup TucuCore
/// Percentile rank, between 0.0 and 100.0
typedef double PercentileRank;

/// \ingroup TucuCore
/// Vector of percentile ranks, each one being between 0.0 and 100.0
typedef std::vector<PercentileRank> PercentileRanks;

/// \ingroup TucuCore
/// \brief Way a dose is administered.
enum class AbsorptionModel { INTRAVASCULAR, EXTRAVASCULAR, INFUSION };

/// \ingroup TucuCore
/// \brief Operations on scheduled intakes.
enum class ScheduledIntakeOp { SKIP, ADD };

/// \ingroup TucuCore
/// \brief Days of the week (in [0, 6]), starting on Sunday.
typedef date::weekday DayOfWeek;
// Days are defined using a #define and not an enum since the library wants
// unsigned variables as input to the constructor
#define SUNDAY       0U
#define MONDAY       1U
#define TUESDAY      2U
#define WEDNESDAY    3U
#define THURSDAY     4U
#define FRIDAY       5U
#define SATURDAY     6U

/// \ingroup TucuCore
/// \brief The type used to represent a serie of residuals.
typedef std::vector<Concentration> Residuals;

/// \ingroup TucuCore
/// \brief The number of points in the cycle.
typedef int CycleSize;

/// \ingroup TucuCore
/// \brief A type representing the Identifier of ProcessingRequests and ProcessingResponses
typedef int RequestResponseId;

/// \ingroup TucuCore
/// \brief A structure to store precomputed exponentials.
//typedef std::vector<Eigen::VectorXd> PrecomputedExponentials;
typedef std::map<int, Eigen::VectorXd> PrecomputedExponentials;

/// \ingroup TucuCore
/// \brief Input operand types.
enum class InputType { BOOL, INTEGER, DOUBLE };


typedef Value Deviation;		    // Used for deviations (e.g. std dev)
typedef std::vector<Deviation> Deviations;  // Used for epsilons array (e.g. std dev)
typedef std::vector<Deviation> Etas;	    // Used for values in eta and error vectors

typedef Eigen::Matrix<Deviation, Eigen::Dynamic, Eigen::Dynamic> OmegaMatrix;
typedef Eigen::Matrix<Deviation, Eigen::Dynamic, 1> Sigma;

typedef Eigen::Matrix<Value, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;
typedef Eigen::Matrix<Value, Eigen::Dynamic, 1> EigenVector;


typedef std::vector<Value> ValueVector;

/// \ingroup TucuCore
/// \brief A type definition for typecasting from std::vector to Eigen::vector
typedef Eigen::Map<const EigenVector> map2EigenVectorType;

// The three following defines could be moved somewhere else
#define isOmegaEmpty(matrix) (matrix.rows() == 0)

#define isOmegaSquare(matrix) (matrix.rows() == matrix.cols())

#define omegaSize(matrix) (matrix.rows())

/// \brief Define the covariate types.
/// - Standard: if no patient variate exist -> use operation in drug model to generate a new value each time one or more
///                                            inputs of the operation are modified
///             if cannot apply operation   -> use default value
///             if >= 1 variate exists      -> if only one value -> use for the entire period
///                                            else              -> interpolate with function defined in
///                                                                 CovariateDefinition, using first observed value for
///                                                                 the interval between start and the first observation
///   \warning Look also at values outside the given period! The period itself limits the range of measures we are
///            interested in, but does not affect the available variates.
/// - AgeInYears: automatic calculation based on birth date, use default if not available, unit = years.
/// - AgeInDays: automatic calculation based on birth date, use default if not available, unit = days.
/// - AgeInMonths: automatic calculation based on birth date, use default if not available, unit = months.
enum class CovariateType {
    Standard = 0,
    AgeInYears,
    AgeInDays,
    AgeInMonths
};

/// \brief Allowed data types.
enum class DataType {
    Int = 0,
    Double,
    Bool,
    Date
};

/// \brief Available interpolation functions.
/// - Direct: when value observed, set it as current value.
/// - Linear: between two occurrences of observed covariates, use linear interpolation.
/// - Sigmoid: between two occurrences of observed covariates, use sigmoidal interpolation.
/// - Tanh: between two occurrences of observed covariates, use hyperbolic tangent interpolation.
enum class InterpolationType
{
    Direct = 0,
    Linear,
    Sigmoid,
    Tanh
};

}
}

#endif // TUCUXI_CORE_DEFINITIONS_H
