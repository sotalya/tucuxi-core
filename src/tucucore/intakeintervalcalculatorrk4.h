#ifndef TUCUXI_CORE_INTAKEINTERVALCALCULATORRK4_H
#define TUCUXI_CORE_INTAKEINTERVALCALCULATORRK4_H

#include <thread>

#include "intakeintervalcalculator.h"

namespace Tucuxi {
namespace Core {


class IntakeIntervalCalculatorRK4 : public IntakeIntervalCalculator
{

public:
    /// \brief Constructor
    IntakeIntervalCalculatorRK4(IPertinentTimesCalculator *_pertinentTimesCalculator) : m_firstCalculation(true),
    m_pertinentTimesCalculator(_pertinentTimesCalculator) {}

    ~IntakeIntervalCalculatorRK4() override;

    ///
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
    ///
    ComputingResult calculateIntakePoints(
        std::vector<Concentrations>& _concentrations,
        TimeOffsets & _times,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        bool _isAll,
        Residuals& _outResiduals,
        bool _isDensityConstant) override;

    ///
    /// \brief Compute one single point at the specified time as well as final residuals
    /// \param _concentrations vector of concentrations.
    /// \param _intakeEvent intake for the cycle (all cyles start with an intake)
    /// \param _parameters Parameters for the cycle (all cycles have constant parameters)
    /// \param _inResiduals Initial residual concentrations
    /// \param _atTime The time of the point of interest
    /// \param _outResiduals Final residual concentrations
    /// \return Returns an indication if the computation was successful
    ///
    ComputingResult calculateIntakeSinglePoint(
        std::vector<Concentrations>& _concentrations,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const Value& _atTime,
        bool _isAll,
        Residuals& _outResiduals) override;

protected:

    ///
    /// \brief Compute concentrations using a specific algorithm
    /// \param _times times for which we need the concentrations
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    ///
    virtual bool computeConcentrations(Eigen::VectorXd &_times, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

    ///
    /// \brief Compute concentrations using a specific algorithm
    /// \param _atTime measure time
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    ///
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

    ///
    /// \brief Set the initial concentrations for each compartment
    /// \param _inResiduals Initial residual concentrations
    /// \param _concentrations vector of initial concentrations set by the function
    ///
    /// In a typical use cas, this function uses the dose and the residuals to set the new concentration.
    ///
    virtual void initConcentrations(const Residuals& _inResiduals, std::vector<double> &_concentrations) = 0;

protected:

    typedef IntakeCalculatorSingleConcentrations SingleConcentrations;

    std::thread::id m_lastThreadId;
    bool m_firstCalculation;

    ///
    /// \brief The calculator for pertinent times
    /// This object is set by the constructor of the subclass depending on its need.
    /// It corresponds to a strategy (or injection) pattern.
    ///
    IPertinentTimesCalculator *m_pertinentTimesCalculator;

};

template<unsigned int ResidualSize, class ImplementationClass>
class IntakeIntervalCalculatorRK4Base : public IntakeIntervalCalculatorRK4
{
public:
    IntakeIntervalCalculatorRK4Base(IPertinentTimesCalculator *_pertinentTimesCalculator) :
        IntakeIntervalCalculatorRK4(_pertinentTimesCalculator) {}

    unsigned int getResidualSize() const override {
        return ResidualSize;
    }

protected:

    int m_nbPoints; /// Number measure points during interval
    Value m_Int; /// Interval time (Hours)

    bool computeConcentrations(Eigen::VectorXd &_times, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        std::vector<Concentrations> concentrations;
        for (unsigned int i = 0; i < ResidualSize; i++) {
            concentrations.push_back(Concentrations(m_nbPoints));
        }

        // compute concentrations
        compute(_times, _inResiduals, concentrations);

        // Get the output residuals
        for (unsigned int i = 0; i < ResidualSize; i++) {
            _outResiduals[i] = concentrations[i][m_nbPoints - 1];
        }

        // Return concentrations of first compartment
        _concentrations[0].assign(concentrations[0].data(), concentrations[0].data() + concentrations[0].size());
        // Return concentrations of other compartments if required
        if (_isAll == true) {
            for (size_t i = 1; i < ResidualSize; i++) {
                _concentrations[i].assign(concentrations[i].data(), concentrations[i].data() + concentrations[i].size());
            }
        }

        // Check that the output residuals are positive
        bool bOk = true;
        for (unsigned int i = 0; i < ResidualSize; i++) {
            bOk &= checkValue(_outResiduals[i] >= 0, "A concentration is negative.");
        }

        return bOk;
    }

    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        std::vector<Concentrations> concentrations;
        for (size_t i = 0; i < ResidualSize; i++) {
            concentrations.push_back(Concentrations(m_nbPoints));
        }

        size_t atTime = static_cast<int>(SingleConcentrations::AtTime);
        size_t atEndInterval = static_cast<int>(SingleConcentrations::AtEndInterval);

        Eigen::VectorXd times(2);
        times[0] = _atTime;
        times[1] = m_Int;
        // compute concentrations
        compute(times, _inResiduals, concentrations);

        // return concentrations (computation with atTime (current time))
        _concentrations[0].push_back(concentrations[0][atTime]);
        // return all compartments if required
        if (_isAll == true) {
            for (size_t i = 1; i < ResidualSize; i++) {
                _concentrations[i].push_back(concentrations[i][atTime]);
            }
        }

        // interval=0 means that it is the last cycle, so final residual = 0
        if (m_Int == 0.0) {
            for (size_t i = 0; i < ResidualSize; i++) {
                concentrations[i][atEndInterval] = 0;
            }
        }

        // Return final residual (computation with m_Int (interval))
        for (size_t i = 0; i < ResidualSize; i++) {
            _outResiduals[i] = concentrations[i][atEndInterval];
        }

        // Checks that the output residuals are positive
        bool bOk = true;
        for (size_t i = 0; i < ResidualSize; i++) {
            bOk &= checkValue(_outResiduals[i] >= 0, "A final residual is negative.");
        }

        return bOk;
    }

    void compute(const Eigen::VectorXd &_times, const Residuals& _inResiduals, std::vector<Concentrations>& _concentrations)
    {
        // By default, we advance minute per minute
        const double stdH = 1.0 / 60.0;

        // h is the delta used and possibly modified at specific iterations
        double h = stdH;

        // The number of points to get corresponds to the size of the times of interest
        Eigen::Index nbPoints = _times.size();

        // The vectors of concentrations
        std::vector<double> concentrations(ResidualSize);

        // We initialize the first concentration
        initConcentrations(_inResiduals, concentrations);

        // The values used by RK4 during one iteration
        std::vector<double> dcdt(ResidualSize);
        std::vector<double> k1(ResidualSize);
        std::vector<double> k2(ResidualSize);
        std::vector<double> k3(ResidualSize);
        std::vector<double> k4(ResidualSize);
        std::vector<double> c(ResidualSize);

        // Time t used for calculating the derivative
        double t = 0.0;

        // cont indicates if the loop is finished or not
        bool cont = true;

        // Next time of interest
        double nextTime = _times[0];

        // Index of the next time of interest
        Eigen::Index nextPoint = 0;

        // Looping on the points to calculate the concentrations of each compartment
        while (cont) {

            // Check if we are ready to get a concentration
            while ((nextTime <= t) && (cont)) {
                for(size_t i = 0; i < ResidualSize; i++) {
                    _concentrations[i][static_cast<size_t>(nextPoint)] = concentrations[i];
                }
                nextPoint ++;
                if (nextPoint >= nbPoints) {
                    cont = false;
                }
                else {
                    nextTime = _times[nextPoint];
                }
            }

            // Adjust h if the next time is close, to reach the exact time point
            if (nextTime - t < stdH) {
                h = nextTime - t;
            }
            else {
                h = stdH;
            }

            // Let's use static inheritance
            static_cast<ImplementationClass*>(this)->derive(t, concentrations, dcdt);

            // Set k1's
            for(size_t i = 0; i < ResidualSize; i++) {
                k1[i] = h * dcdt[i];
                c[i] = concentrations[i] + k1[i] / 2.0;
            }

            static_cast<ImplementationClass*>(this)->derive(t + h / 2.0, c, dcdt);

            // Set k2's
            for(size_t i = 0; i < ResidualSize; i++) {
                k2[i] = h * dcdt[i];
                c[i] = concentrations[i] + k2[i] / 2.0;
            }

            static_cast<ImplementationClass*>(this)->derive(t + h / 2.0, c, dcdt);

            // Set k3's
            for(size_t i = 0; i < ResidualSize; i++) {
                k3[i] = h * dcdt[i];
                c[i] = concentrations[i] + k3[i];
            }

            static_cast<ImplementationClass*>(this)->derive(t + h, c, dcdt);

            // Set k4's
            for(size_t i = 0; i < ResidualSize; i++) {
                k4[i] = h * dcdt[i];
                // and directly compute the concentration of each compartment
                concentrations[i] = concentrations[i] + k1[i] / 6.0 + k2[i] / 3.0 + k3[i] / 3.0 + k4[i] / 6.0;
            }

            t += h;
        }

    }

};


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATORRK4_H
