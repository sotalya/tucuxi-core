#ifndef TEST_UNIT_H
#define TEST_UNIT_H


#include "fructose/fructose.h"

#include "tucucommon/unit.h"
#include "tucucore/definitions.h"

using namespace std;

struct TestUnit : public fructose::test_base<TestUnit>
{
    void unit(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        vector<string> weightUnits
        {
            "ug",
            "mg",
            "g",
            "kg"
        };

        vector<string> heightUnits
        {
            "mm",
            "cm",
            "dm",
            "m"
        };

        vector<string> timeUnits
        {
            "s",
            "min",
            "h",
            "d",
            "m",
            "y"
        };

        vector<string> concentrationUnits
        {
            "g/l",
            "mg/l",
            "ug/l",
            "g/ml",
            "mg/ml",
            "ug/ml"
        };

        vector<string> concentrationTimeUnits
        {
            "g*h/l",
            "mg*h/l",
            "ug*h/l",
            "g*h/ml",
            "mg*h/ml",
            "ug*h/ml",

            "h*g/l",
            "h*mg/l",
            "h*ug/l",
            "h*g/ml",
            "h*mg/ml",
            "h*ug/ml",

            "g*min/l",
            "mg*min/l",
            "ug*min/l",
            "g*min/ml",
            "mg*min/ml",
            "ug*min/ml",

            "min*g/l",
            "min*mg/l",
            "min*ug/l",
            "min*g/ml",
            "min*mg/ml",
            "min*ug/ml"
        };

        vector<string> concentrationMoleUnits
        {
            "mol/l",
            "mmol/l",
            "umol/l",
            "mol/ml",
            "mmol/ml",
            "umol/ml"
        };

        testWeight(weightUnits);

        testHeight(heightUnits);

//        testTime(timeUnits);

//        testConcentration(concentrationUnits);

        testConcentrationTime(concentrationTimeUnits);

        testConcentrationMole(concentrationMoleUnits);






    }

private:

    void testWeight(vector<string>& _weightUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 20.0;

        vector<Tucuxi::Core::Value> empiricalValues
        {
            20.0,
            0.02,
            0.00002,
            0.00000002,
            20000.0,
            20.0,
            0.02,
            0.00002,
            20000000.0,
            20000.0,
            20.0,
            0.02,
            20000000000.0,
            20000000.0,
            20000.0,
            20.0
        };


        uint8_t i = 0;
        for(const auto unit : _weightUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

            for(const auto u : _weightUnits)
            {
                fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(value, unit, u), empiricalValues[i]);
                i++;
            }
        }

    }

    void testHeight(vector<string>& _heightUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 20.0;

        vector<Tucuxi::Core::Value> empiricalValues
        {
            20.0,
            2.0,
            0.2,
            0.02,
            200.0,
            20.0,
            2.0,
            0.2,
            2000.0,
            200.0,
            20.0,
            2.0,
            20000.0,
            2000.0,
            200.0,
            20.0

        };

        uint8_t i = 0;
        for(const auto unit : _heightUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

            for(const auto u : _heightUnits)
            {
                fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Height>(value, unit, u), empiricalValues[i]);
                i++;
            }
        }


    }

    void testTime(vector<string>& _timeUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 31104000.0;

        vector<Tucuxi::Core::Value> empiricalValues
        {
            1.0,
            1.0 / 60,
            1.0 / 3600,
            1.0 / 86400,
            1.0 / 2592000,
            1.0 / 31104000,
        };

        uint8_t i = 0;
        for(const auto unit : _timeUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

            for(const auto u : _timeUnits)
            {
                fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, unit, u), empiricalValues[i]);
                i++;
            }
            empiricalValues[0] = empiricalValues[0]*60.0;
            empiricalValues[1] = empiricalValues[1]*60.0;
            empiricalValues[2] = empiricalValues[2]*60.0;
            empiricalValues[3] = empiricalValues[3]*24.0;
            empiricalValues[4] = empiricalValues[4]*30.0;
            empiricalValues[5] = empiricalValues[5]*12.0;
        }


    }

    void testConcentration(vector<string>& _concentrationUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 20.0;

        vector<Tucuxi::Core::Value> empiricalValues
        {
            20.0,
            20000.0,
            20000000.0,
            0.02,
            20.0,
            20000.0
        };

        uint8_t i = 0;
        for(const auto unit : _concentrationUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

            for(const auto u : _concentrationUnits)
            {
                fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, unit, u), empiricalValues[i]);
                i++;
            }
        }


    }

    void testConcentrationTime(vector<string>& _concentrationTimeUnits){

        Tucuxi::Common::UnitManager unitManager;

        for(const auto unit : _concentrationTimeUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

//            for(const auto u : _concentrationTimeUnits)
//            {

//            }
        }


    }

    void testConcentrationMole(vector<string>& _concentrationMoleUnits){

        Tucuxi::Common::UnitManager unitManager;

        for(const auto unit : _concentrationMoleUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);

//            for(const auto u : _concentrationMoleUnits)
//            {

//            }
        }


    }
};

#endif // TEST_UNIT_H
