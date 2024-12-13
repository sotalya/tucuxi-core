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


#ifndef TUCUXI_CORE_DEFINITIONS_H
#define TUCUXI_CORE_DEFINITIONS_H

/// \defgroup TucuCore Tucuxi core library
/// \brief Tucuxi core classes for the computation of predictions of drug concentrations
/// This module defines all classes needed for:
///    - computing predictions of drug concentrations
///    - computing percentiles for the above predictions
///    - computing suggestions for the adaptation of treatments
///    - modeling relevant data for above computations

#include <iostream>
#include <map>
#include <vector>

#undef min // Prevent problems with date.h
#undef max // Prevent problems with date.h
#include "date/iso_week.h"

#include "tucucommon/basetypes.h"
#include "tucucommon/datetime.h"
#include "tucucommon/unit.h"

#include "Eigen/Dense"

using Tucuxi::Common::TucuUnit; // NOLINT(google-global-names-in-headers)

namespace Tucuxi {
namespace Core {

using Unit = Tucuxi::Common::TucuUnit;

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
typedef Value DoseValue;

/// \ingroup TucuCore
/// \brief The type used to represent a concentration in mg
typedef Value Concentration;

/// \brief The type used to represent a serie of times exrpressed as offsets in hours to the start of a cycle
typedef std::vector<double> TimeOffsets;


#ifdef CHECK_TYPES

// Idea taken from https://stackoverflow.com/questions/6806173/subclass-inherit-standard-containers

/// The behavior of Concentrations is exactly the same as a std::vecto<>.
/// Its purpose is to avoid casts to/from std::vector, for a better code safety.
/// It should not be used in Release mode, as it just adds a small overhead for nothing.

///
/// \ingroup TucuCore
/// \brief The type used to represent a serie of concentrations
///
/// A vector of concentrations, meant to represent a single compartment concentration over time.
///
class Concentrations : private std::vector<Concentration>
{
private:
    // in case I changed to boost or something later, I don't have to update everything below
    typedef std::vector<Concentration> base_vector;

public:
    Concentrations(size_t _size, Concentration _value) : std::vector<Concentration>(_size, _value) {}
    Concentrations(size_t _size = 0) : std::vector<Concentration>(_size) {}
    Concentrations(std::initializer_list<Concentration> init) : std::vector<Concentration>(init) {}

    typedef typename base_vector::size_type size_type;
    typedef typename base_vector::iterator iterator;
    typedef typename base_vector::const_iterator const_iterator;

    using base_vector::operator[];

    using base_vector::assign;
    using base_vector::back;
    using base_vector::begin;
    using base_vector::clear;
    using base_vector::data;
    using base_vector::empty;
    using base_vector::end;
    using base_vector::erase;
    using base_vector::pop_back;
    using base_vector::push_back;
    using base_vector::reserve;
    using base_vector::resize;
    using base_vector::size;
};

///
/// \ingroup TucuCore
/// \brief The MultiCompConcentration class
///
/// The concentrations of multiple compartments at a single time.
///
class MultiCompConcentration : private std::vector<Concentration>
{
private:
    // in case I changed to boost or something later, I don't have to update everything below
    typedef std::vector<Concentration> base_vector;

public:
    MultiCompConcentration(size_t _size, Concentration _value) : std::vector<Concentration>(_size, _value) {}
    MultiCompConcentration(size_t _size = 0) : std::vector<Concentration>(_size) {}
    MultiCompConcentration(std::initializer_list<Concentration> init) : std::vector<Concentration>(init) {}

    typedef typename base_vector::size_type size_type;
    typedef typename base_vector::iterator iterator;
    typedef typename base_vector::const_iterator const_iterator;

    using base_vector::operator[];

    using base_vector::assign;
    using base_vector::back;
    using base_vector::begin;
    using base_vector::clear;
    using base_vector::data;
    using base_vector::empty;
    using base_vector::end;
    using base_vector::erase;
    using base_vector::pop_back;
    using base_vector::push_back;
    using base_vector::reserve;
    using base_vector::resize;
    using base_vector::size;
};

///
/// \ingroup TucuCore
/// \brief The MultiCompConcentrations class
///
/// Embeds an outer vector of compartments. For each compartments the inner vector contains the concentrations of
/// a single compartment.
///
class MultiCompConcentrations : private std::vector<Concentrations>
{
private:
    // in case I changed to boost or something later, I don't have to update everything below
    typedef std::vector<Concentrations> base_vector;

public:
    MultiCompConcentrations(size_t _size, Concentrations _value) : std::vector<Concentrations>(_size, _value) {}
    MultiCompConcentrations(size_t _size = 0) : std::vector<Concentrations>(_size) {}

    typedef typename base_vector::size_type size_type;
    typedef typename base_vector::iterator iterator;
    typedef typename base_vector::const_iterator const_iterator;

    using base_vector::operator[];

    using base_vector::assign;
    using base_vector::back;
    using base_vector::begin;
    using base_vector::clear;
    using base_vector::data;
    using base_vector::empty;
    using base_vector::end;
    using base_vector::erase;
    using base_vector::pop_back;
    using base_vector::push_back;
    using base_vector::reserve;
    using base_vector::resize;
    using base_vector::size;
};

///
/// \ingroup TucuCore
/// \brief The MultiCycleConcentrations class
///
/// Embeds an outer vector of cycles. For each cycle the inner vector contains the concentrations of
/// a single compartment.
///
class MultiCycleConcentrations : private std::vector<Concentrations>
{
private:
    // in case I changed to boost or something later, I don't have to update everything below
    typedef std::vector<Concentrations> base_vector;

public:
    MultiCycleConcentrations(size_t _size, Concentrations _value) : std::vector<Concentrations>(_size, _value) {}
    MultiCycleConcentrations(size_t _size = 0) : std::vector<Concentrations>(_size) {}

    typedef typename base_vector::size_type size_type;
    typedef typename base_vector::iterator iterator;
    typedef typename base_vector::const_iterator const_iterator;

    using base_vector::operator[];

    using base_vector::assign;
    using base_vector::back;
    using base_vector::begin;
    using base_vector::clear;
    using base_vector::data;
    using base_vector::empty;
    using base_vector::end;
    using base_vector::erase;
    using base_vector::pop_back;
    using base_vector::push_back;
    using base_vector::reserve;
    using base_vector::resize;
    using base_vector::size;
};

#else  // CHECK_TYPES
typedef std::vector<Concentration> Concentrations;
typedef std::vector<Concentration> MultiCompConcentration;
typedef std::vector<Concentrations> MultiCompConcentrations;
typedef std::vector<Concentrations> MultiCycleConcentrations;
#endif // CHECK_TYPES

/// \ingroup TucuCore
/// Percentile rank, between 0.0 and 100.0
typedef double PercentileRank;

constexpr double PERCENTILE_RANK_MAX = 100.;
constexpr double PERCENTILE_RANK_MIN = 0.;

/// \ingroup TucuCore
/// Vector of percentile ranks, each one being between 0.0 and 100.0
typedef std::vector<PercentileRank> PercentileRanks;

/// \ingroup TucuCore
/// \brief Way a dose is administered.
enum class AbsorptionModel
{
    Undefined,
    Intravascular,
    Extravascular,
    Infusion,
    ExtravascularLag
};

/// \ingroup TucuCore
/// \brief Operations on scheduled intakes.
enum class ScheduledIntakeOp
{
    Skip,
    Add
};

/// \ingroup TucuCore
/// \brief Days of the week (in [0, 6]), starting on Sunday.
typedef date::weekday DayOfWeek;
// Days are defined using a #define and not an enum since the library wants
// unsigned variables as input to the constructor
constexpr unsigned int SUNDAY = 0U;
constexpr unsigned int MONDAY = 1U;
constexpr unsigned int TUESDAY = 2U;
constexpr unsigned int WEDNESDAY = 3U;
constexpr unsigned int THURSDAY = 4U;
constexpr unsigned int FRIDAY = 5U;
constexpr unsigned int SATURDAY = 6U;

/// \ingroup TucuCore
/// \brief The type used to represent a serie of residuals.
typedef MultiCompConcentration Residuals;

/// \ingroup TucuCore
/// \brief The number of points in the cycle.
typedef size_t CycleSize;

/// \ingroup TucuCore
/// \brief A type representing the Identifier of ComputingRequests and ComputingResponses
typedef std::string RequestResponseId;

/// \ingroup TucuCore
/// \brief A structure to store precomputed exponentials.
//typedef std::vector<Eigen::VectorXd> PrecomputedExponentials;
typedef std::map<int, Eigen::VectorXd> PrecomputedExponentials;

/// \ingroup TucuCore
/// \brief Input operand types.
enum class InputType
{
    BOOL,
    INTEGER,
    DOUBLE
};

typedef Value Deviation;                   // Used for deviations (e.g. std dev)
typedef std::vector<Deviation> Deviations; // Used for epsilons array (e.g. std dev)
typedef std::vector<Deviation> Etas;       // Used for values in eta and error vectors

typedef Eigen::Matrix<Deviation, Eigen::Dynamic, Eigen::Dynamic> OmegaMatrix;
typedef Eigen::Matrix<Deviation, Eigen::Dynamic, 1> Sigma;

typedef Eigen::Matrix<Value, Eigen::Dynamic, Eigen::Dynamic> EigenMatrix;
typedef Eigen::Matrix<Value, Eigen::Dynamic, 1> EigenVector;


typedef std::vector<Value> ValueVector;

/// \ingroup TucuCore
/// \brief A type definition for typecasting from std::vector to Eigen::vector
typedef Eigen::Map<const EigenVector> map2EigenVectorType;

// The three following defines could be moved somewhere else
#define isOmegaEmpty(matrix) ((matrix).rows() == 0) // NOLINT(cppcoreguidelines-macro-usage)

#define isOmegaSquare(matrix) ((matrix).rows() == (matrix).cols()) // NOLINT(cppcoreguidelines-macro-usage)

#define omegaSize(matrix) ((matrix).rows()) // NOLINT(cppcoreguidelines-macro-usage)

/// \brief Define the covariate types.
enum class CovariateType
{

    /// \brief Standard, no particular treatment of the value
    /// - if no patient variate exist -> use operation in drug model to generate a new value each time one or more
    ///                                  inputs of the operation are modified
    ///   if cannot apply operation   -> use default value
    ///   if >= 1 variate exists      -> if only one value -> use for the entire period
    ///                                  else              -> interpolate with function defined in
    ///                                                       CovariateDefinition, using first observed value for
    ///                                                       the interval between start and the first observation
    ///   \warning Look also at values outside the given period! The period itself limits the range of measures we are
    ///            interested in, but does not affect the available variates.
    Standard = 0,

    /// Age in years. Automatic calculation based on birth date, use default if not available, unit = years.
    AgeInYears,

    /// Age in days. Automatic calculation based on birth date, use default if not available, unit = days.
    AgeInDays,

    /// Age in weeks. Automatic calculation based on birth date, use default if not available, unit = weeks.
    AgeInWeeks,

    /// Age in months. Automatic calculation based on birth date, use default if not available, unit = months.
    AgeInMonths,

    /// \brief Sex of the person, based on his administrative data. 1 for male, 0 for female.
    /// Strangely, if Sex is at the second position, the test_covariateextractor.h tests will fail.
    Sex,

    /// \brief Automatic calculation of the dose, based on the intakes.
    /// The unit of the covariate will be used to convert the actual dose to the covariate.
    Dose
};

/// \brief Allowed data types.
enum class DataType
{

    /// An integer
    Int = 0,

    /// A double
    Double,

    /// A boolean
    Bool,

    /// A date (mainly for birthdate)
    Date
};

/// \brief Available interpolation functions.
enum class InterpolationType
{
    /// Direct: when value observed, set it as current value.
    Direct = 0,

    /// Linear: between two occurrences of observed covariates, use linear interpolation.
    Linear,

    /// Sigmoid: between two occurrences of observed covariates, use sigmoidal interpolation.
    Sigmoid,

    /// Tanh: between two occurrences of observed covariates, use hyperbolic tangent interpolation.
    Tanh
};


///
/// \brief The CompartmentInfo class
///
/// This class is meant to represent the organization of prediction curves. As there can be more
/// than one analyte or active moiety, and that there could be compartments concentration in the
/// results, there is a need to identify what is present in the resulting vectors.
///
/// For instance, a SinglePredictionData will embed a vector of CompartmentInfo.
///
/// The id should embed the analyte Id, the activeMoiety Id, or a compartment Id.
///
class CompartmentInfo
{
public:
    /// The type of prediction
    enum class CompartmentType
    {
        /// An active moiety
        ActiveMoiety,
        /// An analyte
        Analyte,
        /// For a single analyte/single active moiety, we face both an analyte and an active moiety
        ActiveMoietyAndAnalyte,
        /// A compartment, not representing an analyte, but an internal compartment of the PK model
        Compartment
    };

    ///
    /// \brief CompartmentInfo constructor
    /// \param _type The type of prediction
    /// \param _id The Id of the prediction
    ///
    CompartmentInfo(CompartmentType _type, std::string _id) : m_type(_type), m_id(std::move(_id)) {}

    ///
    /// \brief gets the Id of this prediction
    /// \return The Id of this prediction
    ///
    std::string getId() const
    {
        return m_id;
    }

    ///
    /// \brief gets the CompartmentType of this prediction
    /// \return The CompartmentType of this prediction
    ///
    CompartmentType getType() const
    {
        return m_type;
    }

protected:
    /// The prediction type
    CompartmentType m_type;

    /// The Id of this prediction
    std::string m_id;
};

} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_DEFINITIONS_H
