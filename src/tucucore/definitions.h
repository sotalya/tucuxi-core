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
/// \brief A structure to store precomputed exponentials.
//typedef std::vector<Eigen::VectorXd> PrecomputedExponentials;
typedef std::map<int, Eigen::VectorXd> PrecomputedExponentials;

/// \ingroup TucuCore
/// \brief Input operand types.
enum class InputType { BOOL, INTEGER, DOUBLE };


typedef Value Deviation;                // Used for deviations (e.g. std dev)
typedef std::vector<Deviation> Deviations;  // Used for epsilons array (e.g. std dev)
typedef std::vector<Deviation> Etas;    // Used for values in eta and error vectors

typedef Eigen::Matrix<Deviation, Eigen::Dynamic, Eigen::Dynamic> OmegaMatrix;
typedef Eigen::Matrix<Deviation, Eigen::Dynamic, 1> Sigma;

typedef Eigen::Matrix<Value, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;
typedef Eigen::Matrix<Value, Eigen::Dynamic, 1> EigenVector;


typedef std::vector<Value> ValueVector;


// The three following defines could be moved somewhere else
#define isOmegaEmpty(matrix) (matrix.rows() == 0)

#define isOmegaSquare(matrix) (matrix.rows() == matrix.cols())

#define omegaSize(matrix) (matrix.rows())

}
}

#endif // TUCUXI_CORE_DEFINITIONS_H
