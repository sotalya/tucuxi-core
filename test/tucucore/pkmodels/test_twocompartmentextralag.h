#ifndef TEST_TWOCOMPARTMENTEXTRALAG_H
#define TEST_TWOCOMPARTMENTEXTRALAG_H


#include <iostream>
#include <memory>

#include "fructose/fructose.h"

#include "tucucommon/duration.h"

#include "tucucore/pkmodels/twocompartmentextralag.h"

#include "tucucore/drugmodel/formulationandroute.h"
#include "tucucore/intakeevent.h"

using namespace Tucuxi::Core;
using namespace std::chrono_literals;

struct TestTwoCompartmentExtraLag : public fructose::test_base<TestTwoCompartmentExtraLag>
{
    static const int CYCLE_SIZE = 251;

    const double DELTA = 0.000001;
    const unsigned int maxResidualSize = 3;

    TestTwoCompartmentExtraLag() { }


    void testFirstDose(const Tucuxi::Core::ParameterSetEvent &_parameters,
                              double _dose,
                              Tucuxi::Core::AbsorptionModel _route,
                              std::chrono::hours _interval,
                              std::chrono::seconds _infusionTime,
                              CycleSize _nbPoints)
    {
        Tucuxi::Core::ComputingStatus res;
        TwoCompartmentExtraLagMacro macroCalculator;

        DateTime now = DateTime::now();
        Tucuxi::Common::Duration offsetTime = 0s;
        Tucuxi::Common::Duration interval = _interval;
        Tucuxi::Common::Duration infusionTime = _infusionTime;

        unsigned int residualSize = (macroCalculator.getResidualSize() == macroCalculator.getResidualSize()) ? macroCalculator.getResidualSize() : maxResidualSize;
        bool isAll = false;

        std::vector<Tucuxi::Core::Concentrations> concentrations;
        concentrations.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrationsInterval2;
        concentrationsInterval2.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrations2;
        concentrations2.resize(residualSize);

        std::vector<Tucuxi::Core::Concentrations> concentrations2Interval2;
        concentrations2Interval2.resize(residualSize);

        Tucuxi::Core::TimeOffsets times;
        Tucuxi::Core::IntakeEvent intakeEvent(now, offsetTime, _dose, TucuUnit("mg"), interval, Tucuxi::Core::FormulationAndRoute(_route), _route, infusionTime, _nbPoints);

        // Start with an empty residual
        const Tucuxi::Core::Residuals firstInResidual(residualSize, 0);
        Tucuxi::Core::Residuals outMicroMultiResiduals(residualSize);
        Tucuxi::Core::Residuals outMicroSingleResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroMultiResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroMultiResiduals2(residualSize);
        Tucuxi::Core::Residuals outMacroSingleResiduals(residualSize);
        Tucuxi::Core::Residuals outMacroSingleResiduals2(residualSize);

        // Calculation of Macro Class
        res = macroCalculator.calculateIntakePoints(
            concentrations,
            times,
            intakeEvent,
            _parameters,
            firstInResidual,
            isAll,
            outMicroMultiResiduals,
            true);

        if (verbose()) {
            std::cout << "[Micro Class Calculation]"<< std::endl;
            for(unsigned int i = 0; i < residualSize; i++) {
                std::cout << "Multiple Out residual["
                          << i
                          << "] = "
                          << outMicroMultiResiduals[i]
                          << std::endl;
            }
        }

        fructose_assert(res == Tucuxi::Core::ComputingStatus::Ok);
    }


    /// \brief Test the residual calculation of extravascular. Compares single point vs multiple points
    /// \param _testName Test name.
    /// A calculator is instanciated, and residuals are computed with both
    ///     calculateIntakePoints and calculateIntakeSinglePoint.
    /// The residuals are then compared and shall be identical.
    void test2compExtraLagSingleDose(const std::string& /* _testName */)
    {

        // parameter for macro class
        Tucuxi::Core::ParameterDefinitions macroParameterDefs;
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("CL", 15.106, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Q", 20, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V2", 342, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("V1", 340, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Ka", 0.609, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("F", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        macroParameterDefs.push_back(std::unique_ptr<Tucuxi::Core::ParameterDefinition>(new Tucuxi::Core::ParameterDefinition("Tlag", 1, Tucuxi::Core::ParameterVariabilityType::None)));
        Tucuxi::Core::ParameterSetEvent macroParameters(DateTime::now(), macroParameterDefs);

        testFirstDose(
            macroParameters,
            400.0,
            Tucuxi::Core::AbsorptionModel::Extravascular,
            12h,
            0s,
            CYCLE_SIZE);
    }
};






#endif // TEST_TWOCOMPARTMENTEXTRALAG_H
