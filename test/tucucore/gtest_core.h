#ifndef GTEST_CORE_H
#define GTEST_CORE_H

#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include "tucucore/concentrationcalculator.h"
#include "tucucore/dosage.h"
#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/drugtreatment/drugtreatment.h"
#include "tucucore/intakeextractor.h"
#include "tucucore/intakeintervalcalculator.h"

#include "pkmodels/constanteliminationbolus.h"
#include "pkmodels/multiconstanteliminationbolus.h"

static const double DEFAULT_PRECISION = 0.00001;

using namespace Tucuxi::Core;

int double_fuzzy_compare(double a, double b, double relative_tolerance, double absolute_tolerance);

bool double_eq_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance);

bool double_ge_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance);

bool double_le_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance);

bool double_ne_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance);

Tucuxi::Core::FormulationAndRoute getInfusionFormulationAndRoute();

Tucuxi::Core::FormulationAndRoute getBolusFormulationAndRoute();

Tucuxi::Core::FormulationAndRoute getExtraFormulationAndRoute();

static const int CYCLE_SIZE = 251;

template<class CalculatorClass>
static void testCalculator(
        const Tucuxi::Core::ParameterSetSeries& _parameters,
        double _dose,
        Tucuxi::Core::AbsorptionModel _route,
        std::chrono::hours _interval,
        std::chrono::seconds _infusionTime,
        CycleSize _nbPoints)
{
    // Compare the result on one interval
    // with ConcentrationCalculator vs directly with the IntakeIntervalCalculator
    {
        Tucuxi::Core::ComputingStatus res;
        std::shared_ptr<IntakeIntervalCalculator> calculator = std::make_shared<CalculatorClass>();

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = calculator->getResidualSize();
        bool isAll = false;

        MultiCompConcentrations concentrations;
        concentrations.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(_route),
                _route,
                infusionTime,
                _nbPoints);

        // std::cout << typeid(calculator).name() << std::endl;

        {
            Tucuxi::Core::Residuals inResiduals(residualSize);
            Tucuxi::Core::Residuals outResiduals(residualSize);

            std::fill(inResiduals.begin(), inResiduals.end(), 0);

            Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
            res = calculator->calculateIntakePoints(
                    concentrations, times, intakeEvent, event, inResiduals, isAll, outResiduals, true);

            ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
        }

        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            DateTime recordFrom = now;
            DateTime recordTo = now + intakeEvent.getInterval();

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);
            ASSERT_EQ(status, ComputingStatus::Ok);
        }

        for (size_t i = 0; i < _nbPoints; i++) {
            Tucuxi::Core::Concentrations concentration2;
            concentration2 = predictionPtr->getValues()[0];
            // std::cout << i <<  " :: " << concentrations[0][i] << " : " << concentration2[i] << std::endl;
            // compare concentrations of compartment 1
            ASSERT_DOUBLE_EQ(concentrations[0][i], concentration2[i]);
        }
    }


    // Use the ConcentrationCalculator in order to calculate multiple cycles
    // and compare it with the sum of individual calculations made with the
    // IntakeIntervalCalculators. Be careful, the interval for the later need
    // to be longer, and the number of points modified accordingly
    //
    // With the new calculation of pertinent times for infusion, this test fails.
    // The test should behave differently in case of infusion
    if (_route != AbsorptionModel::Infusion) {
        size_t nbCycles = 10;

        Tucuxi::Core::ComputingStatus res;
        CalculatorClass calculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = calculator.getResidualSize();
        bool isAll = false;
        MultiCompConcentrations concentrations;
        concentrations.resize(residualSize);
        Tucuxi::Core::TimeOffsets times;
        {
            // Be careful, the intakeEvent embedds the nb of points, but the intervalintakecalculator also. They have to agree

            Tucuxi::Core::IntakeEvent intakeEvent(
                    now,
                    offsetTime,
                    _dose,
                    TucuUnit("mg"),
                    interval * nbCycles,
                    Tucuxi::Core::FormulationAndRoute(_route),
                    _route,
                    infusionTime,
                    (_nbPoints - 1) * nbCycles + 1);

            Tucuxi::Core::Residuals inResiduals(residualSize);
            Tucuxi::Core::Residuals outResiduals(residualSize);

            std::fill(inResiduals.begin(), inResiduals.end(), 0);

            Tucuxi::Core::ParameterSetEvent event = *(_parameters.getAtTime(now));
            res = calculator.calculateIntakePoints(
                    concentrations, times, intakeEvent, event, inResiduals, isAll, outResiduals, true);

#if GTEST_VERBOSE
            for (int testPoint = 0; testPoint < (_nbPoints - 1) * nbCycles + 1; testPoint++) {
                std::cout << "concentration[" << testPoint << "]: " << concentrations[0][testPoint] << std::endl;
            }
#endif

            ASSERT_EQ(res, Tucuxi::Core::ComputingStatus::Ok);
        }

        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();

            DateTime recordFrom = now;
            DateTime recordTo = now + interval * nbCycles;

            for (size_t i = 0; i < nbCycles; i++) {
                Tucuxi::Core::IntakeEvent event(
                        now + interval * i,
                        offsetTime,
                        _dose,
                        TucuUnit("mg"),
                        interval,
                        Tucuxi::Core::FormulationAndRoute(_route),
                        _route,
                        infusionTime,
                        _nbPoints);
                event.setCalculator(calculator2);
                intakeSeries.push_back(event);
            }
            auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);

            ASSERT_EQ(status, ComputingStatus::Ok);

#if GTEST_VERBOSE
            for (int testCycle = 0; testCycle < nbCycles; testCycle++) {
                for (int testNbPoint = 0; testNbPoint < _nbPoints; testNbPoint++) {
                    std::cout << "concentration[" << testCycle << "]"
                              << "[" << testNbPoint << "]"
                              << ": " << predictionPtr->getValues()[testCycle][testNbPoint] << std::endl;
                }
            }
#endif
        }

        // Only works for linear elimination, so do not perform that for some classes
        if (!(typeid(CalculatorClass) == typeid(ConstantEliminationBolus)
              || typeid(CalculatorClass) == typeid(MultiConstantEliminationBolus))) {
            for (size_t cycle = 0; cycle < nbCycles; cycle++) {
                Tucuxi::Core::Concentrations concentration2;
                concentration2 = predictionPtr->getValues()[cycle];
                for (CycleSize i = 0; i < _nbPoints - 1; i++) {
                    double sumConcentration = 0.0;
                    for (size_t c = 0; c < cycle + 1; c++) {
                        sumConcentration += concentrations[0][c * (_nbPoints - 1) + i];
                        // std::cout << c <<  " : " << sumConcentration << " : " << concentrations[0][c * (_nbPoints - 1) + i] << std::endl;
                    }
                    // std::cout << cycle <<  " : " << i << " :: " << predictionPtr->getTimes()[cycle][i] << " . " << sumConcentration << " : " << concentration2[i] << std::endl;
                    ASSERT_PRED4(
                            double_eq_rel_abs,
                            sumConcentration,
                            concentration2[i],
                            DEFAULT_PRECISION,
                            DEFAULT_PRECISION);
                }
            }
        }
    }

    // Create 2 samples and compare the result of computeConcentrations() and pointsAtTime().
    //
    // This test fails for infusion. It should be redesigned according to non linear times
    if (_route != AbsorptionModel::Infusion) {
        CalculatorClass calculator;

        CycleSize nbPoints = 201;
        bool isAll = false;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        Tucuxi::Core::Concentrations concentrations;
        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(
                now,
                offsetTime,
                _dose,
                TucuUnit("mg"),
                interval,
                Tucuxi::Core::FormulationAndRoute(_route),
                _route,
                infusionTime,
                nbPoints);

        Tucuxi::Core::ConcentrationPredictionPtr predictionPtr;
        {
            predictionPtr = std::make_unique<Tucuxi::Core::ConcentrationPrediction>();

            DateTime recordFrom = now;
            DateTime recordTo = recordFrom + interval;

            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);
            auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
            auto status = concentrationCalculator->computeConcentrations(
                    predictionPtr, isAll, recordFrom, recordTo, intakeSeries, _parameters);

            ASSERT_EQ(status, ComputingStatus::Ok);

#if GTEST_VERBOSE
            for (int i = 0; i < nbPoints; i++) {
                std::cout << i << ":" << predictionPtr->getValues()[0][i] << std::endl;
            }
#endif
        }
        {
            Tucuxi::Core::IntakeSeries intakeSeries;
            std::shared_ptr<IntakeIntervalCalculator> calculator2 = std::make_shared<CalculatorClass>();
            intakeEvent.setCalculator(calculator2);
            intakeSeries.push_back(intakeEvent);

            Tucuxi::Core::SampleSeries sampleSeries;
            DateTime date0 = now + interval / 4.0;

            Tucuxi::Core::SampleEvent s0(date0);
            sampleSeries.push_back(s0);
            DateTime date1 = now + interval * 3.0 / 4.0;

            Tucuxi::Core::SampleEvent s1(date1);
            sampleSeries.push_back(s1);

            auto concentrationCalculator = std::make_unique<Tucuxi::Core::ConcentrationCalculator>();
            ComputingStatus res = concentrationCalculator->computeConcentrationsAtTimes(
                    concentrations, isAll, intakeSeries, _parameters, sampleSeries);

            ASSERT_EQ(res, ComputingStatus::Ok);
        }

        size_t n0 = (nbPoints - 1) / 4;
        size_t n1 = (nbPoints - 1) * 3 / 4;

        // compare the result of compartment1
        ASSERT_PRED4(
                double_eq_rel_abs,
                concentrations[0],
                predictionPtr->getValues()[0][n0],
                DEFAULT_PRECISION,
                DEFAULT_PRECISION);
        ASSERT_PRED4(
                double_eq_rel_abs,
                concentrations[1],
                predictionPtr->getValues()[0][n1],
                DEFAULT_PRECISION,
                DEFAULT_PRECISION);
    }

    // Create samples and compare the result of computeConcentrations() and pointsAtTime().
    // This can be done by creating samples corresponding to the number of points asked, and
    // synchronized with the times at which the concentration points are expected
}

std::unique_ptr<DrugTreatment> buildDrugTreatment(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue = DoseValue(200),
        TucuUnit _unit = TucuUnit("mg"),
        int interval = 6,
        unsigned int nbrDoses = 16);

std::unique_ptr<DosageTimeRange> buildDosageTimeRange(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue = DoseValue(200),
        TucuUnit _unit = TucuUnit("mg"),
        int interval = 6,
        unsigned int nbrDoses = 16);
#endif // GTEST_CORE_H
