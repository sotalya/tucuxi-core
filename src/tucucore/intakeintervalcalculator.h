/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
#define TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H

#include <map>

#include "Eigen/Dense"

#include "tucucore/definitions.h"
#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/cachedlogarithms.h"

namespace Tucuxi {
namespace Core {

/// \ingroup TucuCore
/// \brief Base class for the computation of a single intake
/// This class implements the common computation process shared by the different algorithms and
/// delegates specificities to derived classes. Each derived class will implement a specific algorithm.
class IntakeIntervalCalculator
{
public:
    enum class Result {
        Ok,
        BadParameters,
        BadConcentration,
        DensityError
    };

public:
    /// \brief Constructor
    IntakeIntervalCalculator() {}
    
    /// \brief Calculate all points for the given time serie
    /// Variable denisty is used by default, which means IntakeEvent is not constant as the final density 
    /// is stored there. Cornish Fisher cumulants calculated within
    /// @param _concentrations Vector of concentrations
    /// @param _times Vector of times
    /// @param _intakeEvent Intake for the cycle (all cyles start with an intake)
    /// @param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// @param _inResiduals Initial residual concentrations
    /// @param _cycleSize The initial (if set to variable) number of points, always odd
    /// @param _outResiduals Final residual concentrations
    /// @param _isDensityConstant Flag to indicate if initial number of points should be used with a constant density
    /// @return An indication if the computation was successful
    virtual Result calculateIntakePoints(
        Concentrations& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const Parameters& _parameters,
        const Residuals& _inResiduals,
        const CycleSize _cycleSize,
        Residuals& _outResiduals,
        const bool _isDensityConstant);

    /// \brief Compute one single point at the specified time as well as final residuals
    /// @param _concentrations vector of concentrations. 
    /// @param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// @param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// @param _inResiduals Initial residual concentrations
    /// @param _atTime The time of the point of interest
    /// @param _outResiduals Final residual concentrations
    /// @return Returns an indication if the computation was successful
    virtual Result calculateIntakeSinglePoint(
        Concentrations& _concentrations,
        const IntakeEvent& _intakeEvent,
        const Parameters& _parameters,
        const Residuals& _inResiduals,
        const int64& _atTime,
        Residuals& _outResiduals);

protected:
    /// \brief Allows derived classes to make some checks on input data	
    /// @param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// @param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// @return Returns true if inputs are ok
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const Parameters& _parameters) = 0;

    /// \brief Computation of algorithm's variables based on input data
    /// @param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// @param _parameters Parameters for the cycle (all cycles have constant parameters)
    virtual void prepareComputations(const IntakeEvent& _intakeEvent, const Parameters& _parameters) = 0;

    /// \brief Computation of logarithm values that will may be shared by severall successive computations	
    /// @param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// @param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// @param _times Vector of times
    virtual void computeLogarithms(const IntakeEvent& _intakeEvent, const Parameters& _parameters, Eigen::VectorXd& _times) = 0;

    /// \brief Compute concentrations using a specific algorithm
    /// @param _inResiduals Initial residual concentrations
    /// @param _concentrations vector of concentrations.
    /// @param _outResiduals Final residual concentrations
    virtual bool computeConcentrations(const Residuals& _inResiduals, Concentrations& _concentrations, Residuals& _outResiduals) = 0;

    /// \brief Check if a value is correct and log a message if it is not the case
    /// @param _isOk Indicates that the value is correct
    /// @param _errMsg Message to log in case of problem
    bool checkValue(bool _isOk,  const std::string& _errMsg);


protected:
    PrecomputedLogarithms m_precomputedLogarithms;      /// List of precomputed logarithms

private:
    CachedLogarithms m_cache;                           /// The cache of precomputed logarithms
};

}
}

#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
