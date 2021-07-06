/*
* Copyright (C) 2017 Tucuxi SA
*/


#ifndef PKASYMPTOTIC_H
#define PKASYMPTOTIC_H

#include "tucucore/intakeintervalcalculatoranalytical.h"

#include "tucucore/intakeevent.h"

namespace Tucuxi {
namespace Core {

enum class PkAsymptoticExponentials : int { Times };
enum class PkAsymptoticCompartments : int { First };


///
/// \brief The PertinentTimesCalculatorAsymptotic class
/// This class is used by the asymptotic intake calculator. It splits the interval in
/// three pieces, from 0 to peak, peak to down, and down to the end.
///
class PertinentTimesCalculatorAsymptotic : public IPertinentTimesCalculator
{
public:

    ///
    /// \brief setTPeak Sets the peak time
    /// \param _tPeak The peak time, in hours
    ///
    /// This function should be called before calculating the times, so before
    /// calculateTimes is called
    void setTPeak(double _tPeak) {
        m_tPeak = _tPeak;
    }

    ///
    /// \brief Calculates the best times concentration prediction for a asymptotic intakes
    /// \param _intakeEvent The intake event embedding the information
    /// \param _nbPoints The number of points
    /// \param _times The array of times that shall already be allocated
    ///
    /// This function simply divides the interval into 3 subintervals:
    ///     - From 0 to m_tPeak
    ///     - From m_tPeak to (Interval - m_tPeak)
    ///     - From (Interval - m_tPeak) to Interval
    ///
    void calculateTimes(const IntakeEvent& _intakeEvent,
                        Eigen::Index _nbPoints,
                        Eigen::VectorXd& _times) override
    {
        double interval = _intakeEvent.getInterval().toHours();

        if (_nbPoints == 2) {
            _times[0] = 0;
            _times[1] = interval;
            return;
        }

        if (_nbPoints == 1) {
            _times[0] = interval;
            return;
        }

        if (_nbPoints == 3) {
            _times[0] = 0;
            _times[1] = interval / 2.0;
            _times[2] = interval;
            return;
        }

        double tPeak = m_tPeak;

        double middleTime = interval - 2 * tPeak;

        Eigen::Index nbPointsBeforePeak = std::min(_nbPoints, std::max(Eigen::Index{2}, static_cast<Eigen::Index>((tPeak / interval)
                                                                                  * static_cast<double>(_nbPoints))));
        Eigen::Index nbPointsMiddle = _nbPoints - 2 * nbPointsBeforePeak;

        for(Eigen::Index i = 0; i < nbPointsBeforePeak; i++) {
            _times[i] = static_cast<double>(i) / static_cast<double>(nbPointsBeforePeak - 1) * tPeak;
        }

        for(Eigen::Index i = 0; i < nbPointsMiddle; i++) {
            _times[i + nbPointsBeforePeak] = tPeak + static_cast<double>(i + 1) / static_cast<double>(nbPointsMiddle + 1) * middleTime;
        }

        for(Eigen::Index i = 0; i < nbPointsBeforePeak; i++) {
            _times[i + nbPointsBeforePeak + nbPointsMiddle] = interval - tPeak + static_cast<double>(i) / static_cast<double>(nbPointsBeforePeak - 1) * tPeak;
        }

    }

protected:

    /// Time of peak
    double m_tPeak;
};



/// \ingroup TucuCore
/// \brief Intake interval calculator for tests. It is meant to reach an asymptot
///
/// 2 parameters:
///     - Tpeak : time to peak
///     - R     : rate of convergence
/// The equation is the following:
///
/// lastC = residual + (D - residual) * R
/// C(t) = residual + (lastC - residual) * t / Tpeak when t < Tpeak
/// C(t) = lastC - (lastC - residual) * (t - Tpeak) / (Interval - 2 * Tpeak) when Tpeak < t < (Interval - Tpeak)
/// C(t) = residual + (lastC - residual) * (Interval - Tpeak + t - Interval + Tpeak) / Tpeak when (interval - Tpeak) < t
///
/// It allows to test other components with a simple equation:
///     - Easy to calculate AUC
///     - Easy to calculate residual
///     - Verification of steady state
///           The steady state residual concentration is D
///           The steady state AUC is interval * D
///
class PkAsymptotic : public IntakeIntervalCalculatorBase<1, PkAsymptoticExponentials>
{
    INTAKEINTERVALCALCULATOR_UTILS(PkAsymptotic)
public:
    /// \brief Constructor
    PkAsymptotic() : IntakeIntervalCalculatorBase<1, PkAsymptoticExponentials> (new PertinentTimesCalculatorAsymptotic())
    {

    }

    typedef PkAsymptoticExponentials Exponentials;


    static std::vector<std::string> getParametersId()
    {
        return {"TestR", "TestT"};
    }

protected:
    bool checkInputs(const IntakeEvent& _intakeEvent, const ParameterSetEvent& _parameters) override
    {

        if(!checkCondition(_parameters.size() >= 2, "The number of parameters should be equal to 2.")) {
            return false;
        }

        m_D = _intakeEvent.getDose() * 1000;
        m_R = _parameters.getValue(ParameterId::TestR);
        m_TPeak = _parameters.getValue(ParameterId::TestT);
        m_nbPoints = static_cast<Eigen::Index>(_intakeEvent.getNbPoints());
        m_Int = static_cast<int>((_intakeEvent.getInterval()).toHours());

    #ifdef DEBUG
        Tucuxi::Common::LoggerHelper logHelper;

        logHelper.debug("<<Input Values>>");
        logHelper.debug("m_R: {}", m_R);
        logHelper.debug("m_T: {}", m_TPeak);
        logHelper.debug("m_nbPoints: {}", m_nbPoints);
        logHelper.debug("m_Int: {}", m_Int);
    #endif

        // check the inputs
        bool bOK = true;
        bOK &= checkPositiveValue(m_D, "The dose");
        bOK &= checkValidValue(m_TPeak, "The time to peak");
        bOK &= checkValidValue(m_R, "The convergence rate");

        // We have to set the time to peak to allow a correct calculation of times
        static_cast<PertinentTimesCalculatorAsymptotic*>(this->m_pertinentTimesCalculator.get())->setTPeak(m_TPeak);
        return bOK;
    }

    void computeExponentials(Eigen::VectorXd& _times) override
    {
        // Here we just need to store the times
        setExponentials(Exponentials::Times, _times.array());
    }

    bool computeConcentrations(const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        Eigen::VectorXd concentrations;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);


        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return finla residual
        _outResiduals[firstCompartment] = concentrations[m_nbPoints - 1];

        // Return concentraions of first compartment
        _concentrations[firstCompartment].assign(concentrations.data(), concentrations.data() + concentrations.size());
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }

    bool computeConcentration(const Value& _atTime, const Residuals& _inResiduals, bool _isAll, std::vector<Concentrations>& _concentrations, Residuals& _outResiduals) override
    {
        TMP_UNUSED_PARAMETER(_atTime);

        Eigen::VectorXd concentrations;
        size_t firstCompartment = static_cast<size_t>(Compartments::First);
        Eigen::Index atTime = static_cast<Eigen::Index>(SingleConcentrations::AtTime);
        Eigen::Index atEndInterval = static_cast<Eigen::Index>(SingleConcentrations::AtEndInterval);

        // Compute concentrations
        compute(_inResiduals, concentrations);

        // Return concentrations (computation with atTime (current time))
        _concentrations[firstCompartment].push_back(concentrations[atTime]);
        // Only one compartment is existed.
        TMP_UNUSED_PARAMETER(_isAll);

        // interval=0 means that it is the last cycle, so final residual = 0
        if (m_Int == 0.0) {
            concentrations[atEndInterval] = 0;
        }

        // Return final residual (computation with m_Int (interval))
        _outResiduals[firstCompartment] = concentrations[atEndInterval];

        return checkCondition(_outResiduals[firstCompartment] >= 0, "The concentration is negative.");
    }


    void compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations);

    Value m_D;	/// Quantity of drug
    Value m_R;	/// Convergence rate
    Value m_TPeak;	/// Time to peak
    Eigen::Index m_nbPoints; /// Number measure points during interval
    Value m_Int; /// Interval (hours)

private:
    typedef PkAsymptoticCompartments Compartments;
};

inline void PkAsymptotic::compute(const Residuals& _inResiduals, Eigen::VectorXd& _concentrations)
{

    /// lastC = residual + (D - residual) * R
    /// C(t) = residual + (lastC - residual) * t / Tpeak when t < Tpeak
    /// C(t) = lastC - (lastC - residual) * (t - Tpeak) / (Interval - 2 * Tpeak) when Tpeak < t < (Interval - Tpeak)
    /// C(t) = residual + (lastC - residual) * (Interval - Tpeak + t - Interval + Tpeak) / Tpeak when (interval - Tpeak) < t

    _concentrations.setZero(exponentials(Exponentials::Times).size());
    double r0 = _inResiduals[0];
    double lastC = _inResiduals[0] + (m_D - r0) * m_R;
    double diff = lastC - r0;
    for (int i = 0; i < _concentrations.size(); i++) {
        double t = exponentials(Exponentials::Times)[i];
        if (t <= m_TPeak) {
            _concentrations[i] = r0 + diff * (t / m_TPeak);
        }
        else if (t <= (m_Int - m_TPeak)) {
            _concentrations[i] = lastC - diff * ( t - m_TPeak) / (m_Int - 2.0 * m_TPeak);
        }
        else {
            _concentrations[i] = r0 + diff * (t - m_Int + m_TPeak) / m_TPeak;
        }
    }
}

} // namespace Core
} // namespace Tucuxi


#endif // PKASYMPTOTIC_H
