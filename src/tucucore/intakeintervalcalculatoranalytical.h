//@@lisence@@

#ifndef TUCUXI_CORE_INTAKEINTERVALCALCULATORANALYTICAL_H
#define TUCUXI_CORE_INTAKEINTERVALCALCULATORANALYTICAL_H

#include <thread>

#include "intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {

class IntakeIntervalCalculatorAnalytical : public IntakeIntervalCalculator
{

public:
    /// \brief Constructor
    IntakeIntervalCalculatorAnalytical(std::unique_ptr<IPertinentTimesCalculator> _pertinentTimesCalculator)
        : m_firstCalculation(true), m_pertinentTimesCalculator(std::move(_pertinentTimesCalculator))
    {
    }

    ~IntakeIntervalCalculatorAnalytical() override;

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
    ComputingStatus calculateIntakePoints(
            MultiCompConcentrations& _concentrations,
            TimeOffsets& _times,
            const IntakeEvent& _intakeEvent,
            const ParameterSetEvent& _parameters,
            const Residuals& _inResiduals,
            bool _isAll,
            Residuals& _outResiduals,
            bool _isDensityConstant) override;

    /// \brief Compute one single point at the specified time as well as final residuals
    /// \param _concentrations vector of concentrations.
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _inResiduals Initial residual concentrations
    /// \param _atTime The time of the point of interest
    /// \param _outResiduals Final residual concentrations
    /// \return Returns an indication if the computation was successful
    ComputingStatus calculateIntakeSinglePoint(
            MultiCompConcentrations& _concentrations,
            const IntakeEvent& _intakeEvent,
            const ParameterSetEvent& _parameters,
            const Residuals& _inResiduals,
            const Value& _atTime,
            bool _isAll,
            Residuals& _outResiduals) override;

protected:
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
    virtual bool computeConcentrations(
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) = 0;

    /// \brief Compute concentrations using a specific algorithm
    /// \param _atTime measure time
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    virtual bool computeConcentration(
            const Value& _atTime,
            const Residuals& _inResiduals,
            bool _isAll,
            MultiCompConcentrations& _concentrations,
            Residuals& _outResiduals) = 0;


    PrecomputedExponentials m_precomputedExponentials; /// List of precomputed exponentials
    typedef IntakeCalculatorSingleConcentrations SingleConcentrations;

    std::thread::id m_lastThreadId;
    bool m_firstCalculation;

    ///
    /// \brief The calculator for pertinent times
    /// This object is set by the constructor of the subclass depending on its need.
    /// It corresponds to a strategy (or injection) pattern.
    ///
    std::unique_ptr<IPertinentTimesCalculator> m_pertinentTimesCalculator;

private:
    CachedExponentials m_cache; /// The cache of precomputed exponentials
};

template<unsigned int ResidualSize, typename EParameters>
class IntakeIntervalCalculatorBase : public IntakeIntervalCalculatorAnalytical
{
public:
    IntakeIntervalCalculatorBase(std::unique_ptr<IPertinentTimesCalculator> _pertinentTimesCalculator)
        : IntakeIntervalCalculatorAnalytical(std::move(_pertinentTimesCalculator))
    {
    }

    unsigned int getResidualSize() const override
    {
        return ResidualSize;
    }

protected:
    void setExponentials(EParameters _param, Eigen::VectorXd&& _logs)
    {
        m_precomputedExponentials[static_cast<int>(_param)] = _logs;
        //        m_precomputedExponentials = PrecomputedExponentials(4);
        //        m_precomputedExponentials[0] = _logs;
        //        m_precomputedExponentials[0] = Eigen::VectorXd(_logs.rows(), _logs.cols());
        //        m_precomputedExponentials[static_cast<int>(_param)] = _logs;
    }


    //   void setExponentials(EParameters _param, Eigen::VectorXd &_logs) {
    //       m_precomputedExponentials[static_cast<int>(_param)] = _logs;
    //   }

    /*const*/ Eigen::VectorXd& exponentials(EParameters _param)
    {
        return m_precomputedExponentials[static_cast<int>(_param)];
    }
};



} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATORANALYTICAL_H
