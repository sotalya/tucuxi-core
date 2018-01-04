/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
#define TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H

#include <map>
#include <memory>

#include "Eigen/Dense"

#include "tucucore/definitions.h"
//#include "tucucore/intakeevent.h"
#include "tucucore/parameter.h"
#include "tucucore/cachedexponentials.h"

namespace Tucuxi {
namespace Core {

enum class IntakeCalculatorSingleConcentrations : int { AtTime, AtEndInterval };

class IntakeEvent;

class IntakeIntervalCalculator;

///
/// \brief The IntakeIntervalCalculatorCreator class
/// This class is meant to be subclassed in each IntakeIntervalCalculator, thanks to the following macro.
/// It will allow to store creators and use them to create specific calculators.
class IntakeIntervalCalculatorCreator
{
public:
    virtual std::unique_ptr<IntakeIntervalCalculator> create() = 0;
};

/// This macro shall be inserted at the beginning of each class of IntakeIntervalCalculator, passing the class name
/// as parameter to the macro
#define INTAKEINTERVALCALCULATOR_UTILS(entity) \
public: \
    class IntakeCreator : public IntakeIntervalCalculatorCreator \
    { \
        virtual std::unique_ptr<IntakeIntervalCalculator> create() { \
            auto ptr = std::make_unique<entity>(); \
            return std::move(ptr); \
        } \
    }; \
    \
    static std::shared_ptr<IntakeIntervalCalculatorCreator> getCreator() \
    { \
        static std::shared_ptr<IntakeCreator> instance(new IntakeCreator()); \
        return instance; \
    } \
    \
    virtual std::shared_ptr<IntakeIntervalCalculator> getLightClone() { \
        return std::shared_ptr<IntakeIntervalCalculator>(new entity()); \
    }


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

    ///
    /// \brief clone
    /// \return a clone of the object, without copy of the member variables, only the object itself
    ///
    virtual std::shared_ptr<IntakeIntervalCalculator> getLightClone() = 0;
    
    /// \brief Calculate all points for the given time serie
    /// Variable denisty is used by default, which means IntakeEvent is not constant as the final density 
    /// is stored there. Cornish Fisher cumulants calculated within
    /// \param _concentrations Vector of concentrations
    /// \param _times Vector of times
    /// \param _intakeEvent Intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _inResiduals Initial residual concentrations
    /// \param _cycleSize The initial (if set to variable) number of points, always odd
    /// \param _outResiduals Final residual concentrations
    /// \param _isDensityConstant Flag to indicate if initial number of points should be used with a constant density
    /// \return An indication if the computation was successful
    Result calculateIntakePoints(
        std::vector<Concentrations>& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const CycleSize _cycleSize,
	const bool _isAll,
        Residuals& _outResiduals,
        const bool _isDensityConstant);

    /// \brief Compute one single point at the specified time as well as final residuals
    /// \param _concentrations vector of concentrations. 
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _inResiduals Initial residual concentrations
    /// \param _atTime The time of the point of interest
    /// \param _outResiduals Final residual concentrations
    /// \return Returns an indication if the computation was successful
    Result calculateIntakeSinglePoint(
        std::vector<Concentrations>& _concentrations,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const Value& _atTime,
	const bool _isAll,
        Residuals& _outResiduals);

    /// \brief Returns the number of compartments needed for the residuals
    /// \return the number of compartments for the residuals
    virtual unsigned int getResidualSize() const = 0;

protected:
    /// \brief Allows derived classes to make some checks on input data	
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \return Returns true if inputs are ok
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters, CycleSize _cycleSize) = 0;

    /// \brief Computation of exponentials values that will may be shared by severall successive computations
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _times Vector of times
    virtual void computeExponentials(Eigen::VectorXd& _times) = 0;

    /// \brief Compute concentrations using a specific algorithm
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    virtual bool computeConcentrations(const Residuals& _inResiduals, const bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

    /// \brief Compute concentrations using a specific algorithm
    /// \param _atTime measure time
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, const bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

    /// \brief Check if a value is correct and log a message if it is not the case
    /// \param _isOk Indicates that the value is correct
    /// \param _errMsg Message to log in case of problem
    bool checkValue(bool _isOk,  const std::string& _errMsg);

protected:
    PrecomputedExponentials m_precomputedExponentials;      /// List of precomputed exponentials
    typedef IntakeCalculatorSingleConcentrations SingleConcentrations;

private:
    CachedExponentials m_cache;                           /// The cache of precomputed exponentials
};

template<unsigned int ResidualSize, typename EParameters>
class IntakeIntervalCalculatorBase : public IntakeIntervalCalculator
{
public:
    unsigned int getResidualSize() const {
        return ResidualSize;
    }

protected:
    void setExponentials(EParameters _param, Eigen::VectorXd &&_logs) {
        m_precomputedExponentials[static_cast<int>(_param)] = _logs;
//        m_precomputedExponentials = PrecomputedExponentials(4);
//        m_precomputedExponentials[0] = _logs;
//        m_precomputedExponentials[0] = Eigen::VectorXd(_logs.rows(), _logs.cols());
//        m_precomputedExponentials[static_cast<int>(_param)] = _logs;
    }

    Eigen::VectorXd& exponentials(EParameters _param) {
        return m_precomputedExponentials[static_cast<int>(_param)];
//        return m_precomputedExponentials.at(static_cast<int>(_param));
    }

};


}
}

#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
