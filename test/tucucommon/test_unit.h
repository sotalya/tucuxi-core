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
            "day",
            "days",
            "d",
            "w",
            "weeks",
            "week",
            "m",
            "y",
            "year"
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
            "µmol/l",
            "µmol/L",
            "mol/ml",
            "mmol/ml",
            "umol/ml",
            "µmol/ml"
        };

        vector<string> molarMass
        {
            "g/mol",
            "g/umol",
            "kg/mol",
            "kg/umol"
        };

        vector<string> flowRate
        {
            "ml/min",
            "l/min",
            "ml/h",
            "l/h"
        };

        vector<string> temperature
        {
            "celsius"
        };

        vector<string> noUnit
        {
            "-",
            ""
        };

        testWeight(weightUnits);

        testHeight(heightUnits);

        testTime(timeUnits);

        testConcentration(concentrationUnits);

        testConcentrationTime(concentrationTimeUnits);

        testConcentrationMole(concentrationMoleUnits);

        testMolarMass(molarMass);

        testFlowRate(flowRate);




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

        Tucuxi::Core::Value value = 1.0;

        for(const auto unit : _timeUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);
        }

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("s")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("min")), 1.0 / 60.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("h")), 1.0 / 3600.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("day")), 1.0 / 86400);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("day")), 1.0 / 86400);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("d")), 1.0 / 86400);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("w")), 1.0 / 604800);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("weeks")), 1.0 / 604800);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("week")), 1.0 / 604800);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("m")), 1.0 / 2592000);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("y")), 1.0 / 31536000);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("year")), 1.0 / 31536000);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("s")), 60.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("min")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("h")), 1.0 / 60.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("day")), 1.0 / 1440);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("day")), 1.0 / 1440);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("d")), 1.0 / 1440);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("w")), 1.0 / 10080);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("weeks")), 1.0 / 10080);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("week")), 1.0 / 10080);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("m")), 1.0 / 43200);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("y")), 1.0 / 525600);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("year")), 1.0 / 525600);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("s")), 3600.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("min")), 60.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("h")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("day")), 1.0 / 24);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("day")), 1.0 / 24);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("d")), 1.0 / 24);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("w")), 1.0 / 168);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("weeks")), 1.0 / 168);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("week")), 1.0 / 168);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("m")), 1.0 / 720);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("y")), 1.0 / 8760);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("year")), 1.0 / 8760);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("s")), 86400.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("min")), 1440.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("h")), 24.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("d")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("w")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("weeks")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("week")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("m")), 1.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("y")), 1.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("day"), Tucuxi::Common::Unit("year")), 1.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("s")), 86400);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("min")), 1440.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("h")), 24.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("d")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("w")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("weeks")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("week")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("m")), 1.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("y")), 1.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("days"), Tucuxi::Common::Unit("year")), 1.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("s")), 86400.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("min")), 1440.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("h")), 24.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("day")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("d")), 1.0 );
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("w")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("weeks")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("week")), 1.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("m")), 1.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("y")), 1.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("year")), 1.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("s")), 604800.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("min")), 10080.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("h")), 168.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("d")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("w")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("weeks")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("week")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("m")), 7.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("y")), 7.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("year")), 7.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("s")), 604800.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("min")), 10080.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("h")), 168.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("d")), 1.0 * 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("w")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("weeks")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("week")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("m")), 7.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("y")), 7.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("week"), Tucuxi::Common::Unit("year")), 7.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("s")), 604800.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("min")), 10080.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("h")), 168.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("day")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("d")), 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("w")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("weeks")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("week")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("m")), 7.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("y")), 7.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("weeks"), Tucuxi::Common::Unit("year")), 7.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("s")), 2592000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("min")), 43200.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("h")), 720.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("day")), 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("day")), 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("d")), 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("w")), 30.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("weeks")), 30.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("week")), 30.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("m")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("y")), 30.0 / 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("year")), 30.0 / 365.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("s")), 31536000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("min")), 525600.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("h")), 8760.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("day")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("day")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("d")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("w")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("weeks")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("week")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("m")), 365.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("y")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("year")), 1.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("s")), 31536000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("min")), 525600.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("h")), 8760.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("day")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("day")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("d")), 365.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("w")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("weeks")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("week")), 365.0 / 7.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("m")), 365.0 / 30.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("y")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("year"), Tucuxi::Common::Unit("year")), 1.0);


    }

    void testConcentration(vector<string>& _concentrationUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for(const auto unit : _concentrationUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);
        }

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("g/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("mg/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("ug/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("g/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("mg/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/l"), Tucuxi::Common::Unit("ug/ml")), 1000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("g/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("mg/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("ug/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("g/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("mg/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/l"), Tucuxi::Common::Unit("ug/ml")), 1.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("g/l")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("mg/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("ug/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("g/ml")), 0.000000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("mg/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/l"), Tucuxi::Common::Unit("ug/ml")), 0.001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("g/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("mg/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("ug/l")), 1000000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("g/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("mg/ml")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("g/ml"), Tucuxi::Common::Unit("ug/ml")), 1000000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("g/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("mg/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("ug/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("g/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("mg/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("mg/ml"), Tucuxi::Common::Unit("ug/ml")), 1000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("g/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("mg/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("ug/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("g/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("mg/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(value, Tucuxi::Common::Unit("ug/ml"), Tucuxi::Common::Unit("ug/ml")), 1.0);

    }

    void testConcentrationTime(vector<string>& _concentrationTimeUnits){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

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

        Tucuxi::Core::Value value = 1.0;

        for(const auto unit : _concentrationMoleUnits)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);
        }

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("mol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("mmol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("umol/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("µmol/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("µmol/L")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("mol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("mmol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("umol/ml")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/l"), Tucuxi::Common::Unit("µmol/ml")), 1000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("mol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("mmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("umol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("µmol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("µmol/L")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("mol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("mmol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("umol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/l"), Tucuxi::Common::Unit("µmol/ml")), 1.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("mol/l")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("mmol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("umol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("µmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("µmol/L")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("mol/ml")), 0.000000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("mmol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("umol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/l"), Tucuxi::Common::Unit("µmol/ml")), 0.001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("mol/l")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("mmol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("umol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("µmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("µmol/L")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("mol/ml")), 0.000000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("mmol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("umol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/l"), Tucuxi::Common::Unit("µmol/ml")), 0.001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("mol/l")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("mmol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("umol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("µmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("µmol/L")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("mol/ml")), 0.000000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("mmol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("umol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/L"), Tucuxi::Common::Unit("µmol/ml")), 0.001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("mol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("mmol/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("umol/l")), 1000000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("µmol/l")), 1000000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("µmol/L")), 1000000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("mol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("mmol/ml")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("umol/ml")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mol/ml"), Tucuxi::Common::Unit("µmol/ml")), 1000000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("mol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("mmol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("umol/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("µmol/l")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("µmol/L")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("mol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("mmol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("umol/ml")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("mmol/ml"), Tucuxi::Common::Unit("µmol/ml")), 1000.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("mol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("mmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("umol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("µmol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("µmol/L")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("mol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("mmol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("umol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("umol/ml"), Tucuxi::Common::Unit("µmol/ml")), 1.0);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("mol/l")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("mmol/l")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("umol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("µmol/l")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("µmol/L")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("mol/ml")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("mmol/ml")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("umol/ml")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(value, Tucuxi::Common::Unit("µmol/ml"), Tucuxi::Common::Unit("µmol/ml")), 1.0);
    }


    void testMolarMass(vector<string> _molarMass){
        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for(const auto unit : _molarMass)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);
        }

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/mol"), Tucuxi::Common::Unit("g/mol")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/mol"), Tucuxi::Common::Unit("g/umol")), 0.000001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/mol"), Tucuxi::Common::Unit("kg/mol")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/mol"), Tucuxi::Common::Unit("kg/umol")), 0.000000001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/umol"), Tucuxi::Common::Unit("g/mol")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/umol"), Tucuxi::Common::Unit("g/umol")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/umol"), Tucuxi::Common::Unit("kg/mol")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("g/umol"), Tucuxi::Common::Unit("kg/umol")), 0.001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/mol"), Tucuxi::Common::Unit("g/mol")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/mol"), Tucuxi::Common::Unit("g/umol")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/mol"), Tucuxi::Common::Unit("kg/mol")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/mol"), Tucuxi::Common::Unit("kg/umol")), 0.000001);

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/umol"), Tucuxi::Common::Unit("g/mol")), 1000000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/umol"), Tucuxi::Common::Unit("g/umol")), 1000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/umol"), Tucuxi::Common::Unit("kg/mol")), 1000000.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(value, Tucuxi::Common::Unit("kg/umol"), Tucuxi::Common::Unit("kg/umol")), 1.0);

    }

    void testFlowRate(vector<string>& _flowRate){

        Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for(const auto unit : _flowRate)
        {
            fructose_assert_eq(unitManager.isKnown(unit), true);
        }

        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(value, Tucuxi::Common::Unit("ml/min"), Tucuxi::Common::Unit("ml/min")), 1.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(value, Tucuxi::Common::Unit("ml/min"), Tucuxi::Common::Unit("l/min")), 0.001);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(value, Tucuxi::Common::Unit("ml/min"), Tucuxi::Common::Unit("ml/h")), 60.0);
        fructose_assert_double_eq(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(value, Tucuxi::Common::Unit("ml/min"), Tucuxi::Common::Unit("l/h")), 1.0 / 0.06);

    }
};

#endif // TEST_UNIT_H
