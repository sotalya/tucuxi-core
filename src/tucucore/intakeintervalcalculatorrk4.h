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
    ComputingResult calculateIntakePoints(
        std::vector<Concentrations>& _concentrations,
        TimeOffsets & _times,
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
    ComputingResult calculateIntakeSinglePoint(
        std::vector<Concentrations>& _concentrations,
        const IntakeEvent& _intakeEvent,
        const ParameterSetEvent& _parameters,
        const Residuals& _inResiduals,
        const Value& _atTime,
        bool _isAll,
        Residuals& _outResiduals) override;

protected:

    /// \brief Compute concentrations using a specific algorithm
    /// \param _times times for which we need the concentrations
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    virtual bool computeConcentrations(Eigen::VectorXd &_times, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

    /// \brief Compute concentrations using a specific algorithm
    /// \param _atTime measure time
    /// \param _inResiduals Initial residual concentrations
    /// \param _inAll Need concentrations for all compartements or not
    /// \param _concentrations vector of concentrations.
    /// \param _outResiduals Final residual concentrations
    virtual bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) = 0;

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

    bool computeConcentrations(Eigen::VectorXd &_times, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
    {
        std::vector<Concentrations> concentrations;
        for (int i = 0; i < ResidualSize; i++) {
            concentrations.push_back(Concentrations(m_nbPoints));
        }

        // compute concentration1 and 2
        compute(_times, _inResiduals, concentrations);

        for (int i = 0; i < ResidualSize; i++) {
            _outResiduals[i] = concentrations[i][m_nbPoints - 1];
        }

        // Return concentrations of first compartment
        _concentrations[0].assign(concentrations[0].data(), concentrations[0].data() + concentrations[0].size());
        // Return concentrations of other compartments
        if (_isAll == true) {
            for (size_t i = 1; i < ResidualSize; i++) {
                _concentrations[i].assign(concentrations[i].data(), concentrations[i].data() + concentrations[i].size());
            }
        }

        bool bOk = true;
        for (int i = 0; i < ResidualSize; i++) {
            bOk &= checkValue(_outResiduals[i] >= 0, "A concentration is negative.");
        }

        return bOk;
    }

    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals)
    {
        TMP_UNUSED_PARAMETER(_atTime);
        std::vector<Concentrations> concentrations;
        for (size_t i = 0; i < ResidualSize; i++) {
            concentrations.push_back(Concentrations(m_nbPoints));
        }

        size_t atTime = static_cast<int>(SingleConcentrations::AtTime);
        size_t atEndInterval = static_cast<int>(m_nbPoints-1);

        Eigen::VectorXd times(1);
        times[0] = _atTime;
        // compute concentration1 and 2
        compute(times, _inResiduals, concentrations);

        // return concentraions (computation with atTime (current time))
        _concentrations[0].push_back(concentrations[0][atTime]);
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

        bool bOk = true;
        for (size_t i = 0; i < ResidualSize; i++) {
            bOk &= checkValue(_outResiduals[i] >= 0, "A final residual is negative.");
        }

        return bOk;
    }



    int m_nbPoints; /// Number measure points during interval
    Value m_Int; /// Interval time (Hours)

    void compute(const Eigen::VectorXd &_times, const Residuals& _inResiduals, std::vector<Concentrations>& _concentrations)
    {
        const double h = m_Int/m_nbPoints;


        std::vector<double> concentrations(ResidualSize);

        initConcentrations(_inResiduals, concentrations);


        /* Looping the rest of the points to calculate the conventrations
             * of each compartment */
        std::vector<double> dcdt(ResidualSize);
        std::vector<double> k1(ResidualSize);
        std::vector<double> k2(ResidualSize);
        std::vector<double> k3(ResidualSize);
        std::vector<double> k4(ResidualSize);
        std::vector<double> c(ResidualSize);
        double t = 0.0;

        for (auto i = 0; (i + 1) < m_nbPoints; i = i + 1) {

            // Let's use static inheritance
            static_cast<ImplementationClass*>(this)->derive(t, concentrations, dcdt);

            /* Set k1's*/
            for(size_t i = 0; i < ResidualSize; i++) {
                k1[i] = h * dcdt[i];
                c[i] = concentrations[i] + k1[i] / 2.0;
            }

            static_cast<ImplementationClass*>(this)->derive(t + h / 2.0, c, dcdt);

            /* Set k2's*/
            for(size_t i = 0; i < ResidualSize; i++) {
                k2[i] = h * dcdt[i];
                c[i] = concentrations[i] + k2[i] / 2.0;
            }

            static_cast<ImplementationClass*>(this)->derive(t + h / 2.0, c, dcdt);

            /* Set k3's*/
            for(size_t i = 0; i < ResidualSize; i++) {
                k3[i] = h * dcdt[i];
                c[i] = concentrations[i] + k3[i];
            }

            static_cast<ImplementationClass*>(this)->derive(t + h, c, dcdt);

            /* Set k4's*/
            for(size_t i = 0; i < ResidualSize; i++) {
                k4[i] = h * dcdt[i];
                c[i] = concentrations[i] + k1[i] / 6.0 + k2[i] / 3.0 + k3[i] / 3.0 + k4[i] / 6.0;
            }

            for(size_t i = 0; i < ResidualSize; i++) {
                _concentrations[i][i+1] = c[i];
            }

            t += h;
        }

    }

};


} // namespace Core
} // namespace Tucuxi


#endif // TUCUXI_CORE_INTAKEINTERVALCALCULATORRK4_H
