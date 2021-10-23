/*
* Copyright (C) 2017 Tucuxi SA
*/

#ifndef TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
#define TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H

#include <map>
#include <memory>

#include "Eigen/Dense"

#include "tucucore/computingservice/computingresult.h"
#include "tucucore/definitions.h"
#include "tucucore/parameter.h"
#include "tucucore/cachedexponentials.h"

namespace Tucuxi {
namespace Core {

enum class IntakeCalculatorSingleConcentrations : int { AtTime = 0, AtEndInterval };

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

    virtual ~IntakeIntervalCalculatorCreator() = default;
};

/// This macro shall be inserted at the beginning of each class of IntakeIntervalCalculator, passing the class name
/// as parameter to the macro
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INTAKEINTERVALCALCULATOR_UTILS(entity) \
public: \
    class IntakeCreator : public IntakeIntervalCalculatorCreator \
    { \
        std::unique_ptr<IntakeIntervalCalculator> create() override { \
            return std::make_unique<entity>(); \
        } \
    }; \
    \
    static std::shared_ptr<IntakeIntervalCalculatorCreator> getCreator() \
    { \
        static std::shared_ptr<IntakeCreator> instance = std::make_shared<IntakeCreator>(); \
        return instance; \
    } \
    \
    std::shared_ptr<IntakeIntervalCalculator> getLightClone() override { \
        return std::shared_ptr<IntakeIntervalCalculator>(new entity()); \
    }

//#ifndef NDEBUG
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_CALCULATEINTAKEPOINTS_INPUTS \
{ \
    if (_inResiduals.size() < getResidualSize()) { \
        throw std::runtime_error("[IntakeIntervalCalculator] Input residual vector size too short"); \
    } \
    if (_outResiduals.size() < getResidualSize()) { \
        throw std::runtime_error("[IntakeIntervalCalculator] Output residual vector size too short"); \
    } \
}
/*
 * The next checks would be relevant if the concentrations are allocated
 * outside of the intake calculator. But that's actually not the case
    if (_concentrations.size() < m_nbAnalytes) { \
        throw std::runtime_error("[IntakeIntervalCalculator] _concentrations vector does not have a size corresponding to the number of analytes"); \
    } \
    for (size_t i = 0; i < m_nbAnalytes; i++) { \
        if (_concentrations[i].size() < _intakeEvent.getNbPoints()) { \
            throw std::runtime_error("[IntakeIntervalCalculator] _concentrations vector does not have a correct number of points"); \
        } \
    } \
}
*/
//#endif

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECK_CALCULATEINTAKESINGLEPOINT_INPUTS \
{ \
    if (_inResiduals.size() < getResidualSize()) { \
        throw std::runtime_error("[IntakeIntervalCalculator] Input residual vector size too short"); \
    } \
    if (_outResiduals.size() < getResidualSize()) { \
        throw std::runtime_error("[IntakeIntervalCalculator] Output residual vector size too short"); \
    } \
}

/// \ingroup TucuCore
/// \brief Base class for the computation of a single intake
/// This class implements the common computation process shared by the different algorithms and
/// delegates specificities to derived classes. Each derived class will implement a specific algorithm.
class IntakeIntervalCalculator
{

public:
    /// \brief Constructor
    IntakeIntervalCalculator() : m_loggingErrors(true) {}

    virtual ~IntakeIntervalCalculator();

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
    /// \param _outResiduals Final residual concentrations
    /// \param _isDensityConstant Flag to indicate if initial number of points should be used with a constant density
    /// \return An indication if the computation was successful
    virtual ComputingStatus calculateIntakePoints(
        std::vector<Concentrations>& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        bool _isAll,
        Residuals& _outResiduals,
        bool _isDensityConstant) = 0;

    /// \brief Compute one single point at the specified time as well as final residuals
    /// \param _concentrations vector of concentrations. 
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _inResiduals Initial residual concentrations
    /// \param _atTime The time of the point of interest
    /// \param _outResiduals Final residual concentrations
    /// \return Returns an indication if the computation was successful
    virtual ComputingStatus calculateIntakeSinglePoint(   
        std::vector<Concentrations>& _concentrations,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const Value& _atTime,
        bool _isAll,
        Residuals& _outResiduals) = 0;

    /// \brief Returns the number of compartments needed for the residuals
    /// \return the number of compartments for the residuals
    virtual unsigned int getResidualSize() const = 0;

    ///
    /// \brief Returns the number of analytes computed by this calculator
    /// \return The number of analytes computed
    ///
    unsigned int getNbAnalytes() const;

protected:
    /// \brief Allows derived classes to make some checks on input data	
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \return Returns true if inputs are ok
    virtual bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) = 0;

    /// \brief Check if a value is correct and log a message if it is not the case
    /// \param _isOk Indicates that the value is correct
    /// \param _errMsg Message to log in case of problem
    bool checkCondition(bool _isOk,  const std::string& _errMsg);

    /// \brief Check if a value is a number and not infinity and log a message if it is not the case
    /// \param _value Value to be tested
    /// \param _valueName Name of the value to log in case of problem
    bool checkValidValue(Value _value,  const std::string& _valueName);

    /// \brief Check if a value is positive and log a message if it is not the case
    /// \param _value Value to be tested
    /// \param _valueName Name of the value to log in case of problem
    bool checkPositiveValue(Value _value,  const std::string& _valueName);

    /// \brief Check if a value is strictly positive and log a message if it is not the case
    /// \param _value Value to be tested
    /// \param _valueName Name of the value to log in case of problem
    bool checkStrictlyPositiveValue(Value _value,  const std::string& _valueName);

protected:
    typedef IntakeCalculatorSingleConcentrations SingleConcentrations;

    /// Indicates if we shall log errors or not. Unsed to disable it for single points
    /// calculation. Mainly because it is used by LogLikelihood and that parameters
    /// can be wrong because of big etas
    bool m_loggingErrors;

    /// By default the number of analytes is 1, as it is the most common case
    /// If there are more than one analyte, then the IntakeIntervalCalculator can set
    /// this value in its constructor
    unsigned int m_nbAnalytes{1};

};

///
/// \brief The IPertinentTimesCalculator class
///
class IPertinentTimesCalculator
{
public:

    ///
    /// \brief Calculates the best times concentration prediction for a specific intake
    /// \param _intakeEvent The intake event embedding the information
    /// \param _nbPoints The number of points
    /// \param _times The array of times that shall already be allocated
    ///
    /// This function calculates the best times for _nbPoints and a specific intake.
    /// It is meant to be subclassed for specific needs.
    ///
    virtual void calculateTimes(const IntakeEvent& _intakeEvent,
                        Eigen::Index _nbPoints,
                        Eigen::VectorXd& _times) = 0;

    /// Virtual destructor
    virtual ~IPertinentTimesCalculator() = default;
};

///
/// \brief The PertinentTimesCalculatorStandard class
/// This class is used by extra and bolus intake calculators. It splits the interval in
/// linear time.
///
class PertinentTimesCalculatorStandard : public IPertinentTimesCalculator
{
public:

    ///
    /// \brief Calculates the best times concentration prediction for a standard intakes
    /// \param _intakeEvent The intake event embedding the information
    /// \param _nbPoints The number of points
    /// \param _times The array of times that shall already be allocated
    ///
    /// This function simply divides the interval in (_nbPoints-1) subintervals.
    ///
    void calculateTimes(const IntakeEvent& _intakeEvent,
                        Eigen::Index _nbPoints,
                        Eigen::VectorXd& _times) override;
};

///
/// \brief The PertinentTimesCalculatorInfusion class
/// This class is used by infusion intake calculators to split the interval in during infusion vs
/// post infusion times.
///
class PertinentTimesCalculatorInfusion : public IPertinentTimesCalculator
{
public:

    ///
    /// \brief Calculates the best times concentration prediction for infusion intakes
    /// \param _intakeEvent The intake event embedding the information
    /// \param _nbPoints The number of points
    /// \param _times The array of times that shall already be allocated
    ///
    /// This function divides the interval in two : during infusion, and post infusion.
    /// It tries to do the best to keep the time deltas as linear as possible.
    ///
    void calculateTimes(const IntakeEvent& _intakeEvent,
                        Eigen::Index _nbPoints,
                        Eigen::VectorXd& _times) override;
};



} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATOR_H
