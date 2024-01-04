//@@license@@

#include <vector>

#include <gtest/gtest.h>

#include "tucucommon/unit.h"

#include "tucucore/definitions.h"

using namespace std;

TEST (Common_Unit, WeightUnit){
    // clang-format off
    vector<string> weightUnits
        {
            "ng",
            "µg",
            "ug",
            "mg",
            "g",
            "kg"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 20.0;

        for (const auto& unit : weightUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("ug"), Tucuxi::Common::TucuUnit("ug")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("ug"), Tucuxi::Common::TucuUnit("mg")),
                0.02);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("ug"), Tucuxi::Common::TucuUnit("g")),
                0.00002);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("ug"), Tucuxi::Common::TucuUnit("kg")),
                0.00000002);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("mg"), Tucuxi::Common::TucuUnit("ug")),
                20000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("mg"), Tucuxi::Common::TucuUnit("mg")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("mg"), Tucuxi::Common::TucuUnit("g")),
                0.02);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("mg"), Tucuxi::Common::TucuUnit("kg")),
                0.00002);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("ug")),
                20000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("mg")),
                20000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("g")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("kg")),
                0.02);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("kg"), Tucuxi::Common::TucuUnit("ug")),
                20000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("kg"), Tucuxi::Common::TucuUnit("mg")),
                20000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("kg"), Tucuxi::Common::TucuUnit("g")),
                20000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("kg"), Tucuxi::Common::TucuUnit("kg")),
                20.0);

        // The following tests are testing units added later. It is sufficient to convert to the
        // base unit g

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("ng"), Tucuxi::Common::TucuUnit("g")),
                0.00000002);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("ng")),
                20000000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("µg"), Tucuxi::Common::TucuUnit("g")),
                0.00002);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Weight>(
                        value, Tucuxi::Common::TucuUnit("g"), Tucuxi::Common::TucuUnit("µg")),
                20000000.0);
}

TEST (Common_Unit, HeightUnit){
    // clang-format off
    vector<string> lengthUnits
        {
            "mm",
            "cm",
            "dm",
            "m"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 20.0;

        for (const auto& unit : lengthUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("mm"), Tucuxi::Common::TucuUnit("mm")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("mm"), Tucuxi::Common::TucuUnit("cm")),
                2.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("mm"), Tucuxi::Common::TucuUnit("dm")),
                0.2);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("mm"), Tucuxi::Common::TucuUnit("m")),
                0.02);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("cm"), Tucuxi::Common::TucuUnit("mm")),
                200.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("cm"), Tucuxi::Common::TucuUnit("cm")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("cm"), Tucuxi::Common::TucuUnit("dm")),
                2.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("cm"), Tucuxi::Common::TucuUnit("m")),
                0.2);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("dm"), Tucuxi::Common::TucuUnit("mm")),
                2000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("dm"), Tucuxi::Common::TucuUnit("cm")),
                200.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("dm"), Tucuxi::Common::TucuUnit("dm")),
                20.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("dm"), Tucuxi::Common::TucuUnit("m")),
                2.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("m"), Tucuxi::Common::TucuUnit("mm")),
                20000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("m"), Tucuxi::Common::TucuUnit("cm")),
                2000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("m"), Tucuxi::Common::TucuUnit("dm")),
                200.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Length>(
                        value, Tucuxi::Common::TucuUnit("m"), Tucuxi::Common::TucuUnit("m")),
                20.0);
}

TEST (Common_Unit, TimeUnit){
    // clang-format off
    vector<string> timeUnits
        {
            "s",
            "min",
            "h",
//            "day",
//            "days",
            "d",
            "w",
//            "weeks",
//            "week",
            "month",
            "y"
//            "year"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : timeUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("s")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("min")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("h")),
                1.0 / 3600.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("day")), 1.0 / 86400);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("day")), 1.0 / 86400);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("d")),
                1.0 / 86400);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("w")),
                1.0 / 604800);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("weeks")), 1.0 / 604800);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("week")), 1.0 / 604800);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("month")),
                1.0 / 2592000);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("s"), Tucuxi::Common::TucuUnit("y")),
                1.0 / 31536000);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("s"), Tucuxi::Common::Unit("year")), 1.0 / 31536000);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("s")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("min")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("h")),
                1.0 / 60.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("day")), 1.0 / 1440);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("day")), 1.0 / 1440);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("d")),
                1.0 / 1440);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("w")),
                1.0 / 10080);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("weeks")), 1.0 / 10080);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("week")), 1.0 / 10080);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("month")),
                1.0 / 43200);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("min"), Tucuxi::Common::TucuUnit("y")),
                1.0 / 525600);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("min"), Tucuxi::Common::Unit("year")), 1.0 / 525600);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("s")),
                3600.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("min")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("h")),
                1.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("day")), 1.0 / 24);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("day")), 1.0 / 24);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("d")),
                1.0 / 24);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("w")),
                1.0 / 168);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("weeks")), 1.0 / 168);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("week")), 1.0 / 168);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("month")),
                1.0 / 720);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("h"), Tucuxi::Common::TucuUnit("y")),
                1.0 / 8760);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("h"), Tucuxi::Common::Unit("year")), 1.0 / 8760);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("s")),
                86400.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("min")),
                1440.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("h")),
                24.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("day")), 1.0 );
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("day")), 1.0 );
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("d")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("w")),
                1.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("weeks")), 1.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("week")), 1.0 / 7.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("month")),
                1.0 / 30.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("d"), Tucuxi::Common::TucuUnit("y")),
                1.0 / 365.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("d"), Tucuxi::Common::Unit("year")), 1.0 / 365.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("s")),
                604800.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("min")),
                10080.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("h")),
                168.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("day")), 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("day")), 7.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("d")),
                7.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("w")),
                1.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("weeks")), 1.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("week")), 1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("month")),
                7.0 / 30.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("w"), Tucuxi::Common::TucuUnit("y")),
                7.0 / 365.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("w"), Tucuxi::Common::Unit("year")), 7.0 / 365.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("s")),
                2592000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("min")),
                43200.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("h")),
                720.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("day")), 30.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("day")), 30.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("d")),
                30.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("w")),
                30.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("weeks")), 30.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("week")), 30.0 / 7.0);
// ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("m")), 1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("month"), Tucuxi::Common::TucuUnit("y")),
                30.0 / 365.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("m"), Tucuxi::Common::Unit("year")), 30.0 / 365.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("s")),
                31536000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("min")),
                525600.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("h")),
                8760.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("day")), 365.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("day")), 365.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("d")),
                365.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("w")),
                365.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("weeks")), 365.0 / 7.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("week")), 365.0 / 7.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("month")),
                365.0 / 30.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(
                        value, Tucuxi::Common::TucuUnit("y"), Tucuxi::Common::TucuUnit("y")),
                1.0);
//        ASSERT_DOUBLE_EQ(unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Time>(value, Tucuxi::Common::Unit("y"), Tucuxi::Common::Unit("year")), 1.0);
}

TEST (Common_Unit, ConcentrationUnit){
    // clang-format off
    vector<string> concentrationUnits
        {
            "g/l",
            "mg/l",
            "µg/l",
            "ug/l",
            "ng/l",
            "g/dl",
            "mg/dl",
            "µg/dl",
            "ug/dl",
            "ng/dl",
            "mg/cl",
            "µg/cl",
            "ug/cl",
            "ng/cl",
            "g/ml",
            "mg/ml",
            "µg/ml",
            "ug/ml",
            "ng/ml"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : concentrationUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ug/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/l"), Tucuxi::Common::TucuUnit("ug/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("g/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("mg/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("ug/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("g/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("mg/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/l"), Tucuxi::Common::TucuUnit("ug/ml")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("g/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("mg/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("ug/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("g/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("mg/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/ml"), Tucuxi::Common::TucuUnit("ug/ml")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/ml"), Tucuxi::Common::TucuUnit("ug/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/ml"), Tucuxi::Common::TucuUnit("ug/ml")),
                1.0);

        // The following tests are testing units added later. It is sufficient to convert to the
        // base unit g/l

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("µg/l"), Tucuxi::Common::TucuUnit("g/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("µg/l")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ng/l"), Tucuxi::Common::TucuUnit("g/l")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ng/l")),
                1000000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/dl"), Tucuxi::Common::TucuUnit("g/l")),
                10.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("g/dl")),
                0.1);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("µg/dl"), Tucuxi::Common::TucuUnit("g/l")),
                0.00001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("µg/dl")),
                100000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/dl"), Tucuxi::Common::TucuUnit("g/l")),
                0.00001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ug/dl")),
                100000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ng/dl"), Tucuxi::Common::TucuUnit("g/l")),
                0.00000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ng/dl")),
                100000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("mg/cl"), Tucuxi::Common::TucuUnit("g/l")),
                0.1);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("mg/cl")),
                10.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("µg/cl"), Tucuxi::Common::TucuUnit("g/l")),
                0.0001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("µg/cl")),
                10000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ug/cl"), Tucuxi::Common::TucuUnit("g/l")),
                0.0001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ug/cl")),
                10000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ng/cl"), Tucuxi::Common::TucuUnit("g/l")),
                0.0000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ng/cl")),
                10000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("µg/ml"), Tucuxi::Common::TucuUnit("g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("µg/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("ng/ml"), Tucuxi::Common::TucuUnit("g/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Concentration>(
                        value, Tucuxi::Common::TucuUnit("g/l"), Tucuxi::Common::TucuUnit("ng/ml")),
                1000000.0);
}

TEST (Common_Unit, ConcentrationTimeUnit){
    // clang-format off
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
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : concentrationTimeUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00000006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00000006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                0.06);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*h/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*h/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*h/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00000006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00000006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                0.06);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*g/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*mg/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("h*ug/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0 / 60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0 / 1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0 / 1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0 / 1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("g*min/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("mg*min/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("ug*min/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                60000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                60000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                60000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                60.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                60000.0 / 60.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0 / 1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0 / 1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0 / 1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0 / 1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/l"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0 / 1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*g/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*mg/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("g*h/l")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("mg*h/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("ug*h/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("g*h/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("mg*h/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("ug*h/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*g/l")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*mg/l")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*ug/l")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*g/ml")),
                0.000001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*mg/ml")),
                0.001 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("h*ug/ml")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("g*min/l")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("mg*min/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("ug*min/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("g*min/ml")),
                0.00006 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("mg*min/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("ug*min/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*g/l")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*mg/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*ug/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*g/ml")),
                0.00006 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*mg/ml")),
                0.06 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::ConcentrationTime>(
                        value, Tucuxi::Common::TucuUnit("min*ug/ml"), Tucuxi::Common::TucuUnit("min*ug/ml")),
                1.0);
}

TEST (Common_Unit, ConcentrationMoleUnit){
    // clang-format off
    vector<string> concentrationMoleUnits
        {
            "mol/l",
            "mmol/l",
            "umol/l",
            "µmol/l",
            "µmol/l",
            "µmol/L",
            "mol/ml",
            "mmol/ml",
            "umol/ml",
            "µmol/ml"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : concentrationMoleUnits) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("mol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("mmol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("umol/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("mmol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("umol/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/l"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("mol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("mmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("umol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("umol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/l"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("mol/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("mmol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("umol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("µmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("µmol/L")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("umol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/l"), Tucuxi::Common::TucuUnit("µmol/ml")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("mol/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("mmol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("umol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("µmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("µmol/L")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("umol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/l"), Tucuxi::Common::TucuUnit("µmol/ml")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("mol/l")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("mmol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("umol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("µmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("µmol/L")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("umol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/L"), Tucuxi::Common::TucuUnit("µmol/ml")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("mol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("mmol/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("umol/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("mol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("mmol/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("umol/ml")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mol/ml"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("mol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("mmol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("umol/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("mmol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("umol/ml")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("mmol/ml"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("mol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("mmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("umol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("umol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("umol/ml"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("mol/l")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("mmol/l")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("umol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("µmol/l")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("µmol/L")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("mol/ml")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("mmol/ml")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("umol/ml")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MoleConcentration>(
                        value, Tucuxi::Common::TucuUnit("µmol/ml"), Tucuxi::Common::TucuUnit("µmol/ml")),
                1.0);
}

TEST (Common_Unit, MolarMassUnit){
    // clang-format off
    vector<string> molarMass
        {
            "kg/mol",
            "g/mol",
            "mg/mol",
            "ug/mol",
            "µg/mol",
            "ug/umol",
            "µg/umol",
            "g/umol",
            "kg/umol"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : molarMass) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("g/mol")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("g/umol")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("kg/mol")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("kg/umol")),
                0.000000001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/umol"), Tucuxi::Common::TucuUnit("g/mol")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/umol"), Tucuxi::Common::TucuUnit("g/umol")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/umol"), Tucuxi::Common::TucuUnit("kg/mol")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/umol"), Tucuxi::Common::TucuUnit("kg/umol")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/mol"), Tucuxi::Common::TucuUnit("g/mol")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/mol"), Tucuxi::Common::TucuUnit("g/umol")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/mol"), Tucuxi::Common::TucuUnit("kg/mol")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/mol"), Tucuxi::Common::TucuUnit("kg/umol")),
                0.000001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/umol"), Tucuxi::Common::TucuUnit("g/mol")),
                1000000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/umol"), Tucuxi::Common::TucuUnit("g/umol")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/umol"), Tucuxi::Common::TucuUnit("kg/mol")),
                1000000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("kg/umol"), Tucuxi::Common::TucuUnit("kg/umol")),
                1.0);

        // The following tests are testing units added later. It is sufficient to convert to the
        // base unit g/mol

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("mg/mol"), Tucuxi::Common::TucuUnit("g/mol")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("mg/mol")),
                1000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("ug/mol"), Tucuxi::Common::TucuUnit("g/mol")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("ug/mol")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("µg/mol"), Tucuxi::Common::TucuUnit("g/mol")),
                0.000001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("µg/mol")),
                1000000.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("ug/umol"), Tucuxi::Common::TucuUnit("g/mol")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("ug/umol")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("µg/umol"), Tucuxi::Common::TucuUnit("g/mol")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("µg/umol")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("mg/umol"), Tucuxi::Common::TucuUnit("g/mol")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::MolarMass>(
                        value, Tucuxi::Common::TucuUnit("g/mol"), Tucuxi::Common::TucuUnit("mg/umol")),
                0.001);
}

TEST (Common_Unit, FlowRateUnit){
    // clang-format off
    vector<string> flowRate
        {
            "ml/min",
            "l/min",
            "ml/h",
            "l/h"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : flowRate) {
            ASSERT_EQ(unitManager.isKnown(unit), true);
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/min"), Tucuxi::Common::TucuUnit("ml/min")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/min"), Tucuxi::Common::TucuUnit("l/min")),
                0.001);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/min"), Tucuxi::Common::TucuUnit("ml/h")),
                60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/min"), Tucuxi::Common::TucuUnit("l/h")),
                0.06);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/min"), Tucuxi::Common::TucuUnit("ml/min")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/min"), Tucuxi::Common::TucuUnit("l/min")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/min"), Tucuxi::Common::TucuUnit("ml/h")),
                60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/min"), Tucuxi::Common::TucuUnit("l/h")),
                60);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/h"), Tucuxi::Common::TucuUnit("ml/min")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/h"), Tucuxi::Common::TucuUnit("l/min")),
                1.0 / 60000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/h"), Tucuxi::Common::TucuUnit("ml/h")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("ml/h"), Tucuxi::Common::TucuUnit("l/h")),
                0.001);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/h"), Tucuxi::Common::TucuUnit("ml/min")),
                1000.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/h"), Tucuxi::Common::TucuUnit("l/min")),
                1.0 / 60.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/h"), Tucuxi::Common::TucuUnit("ml/h")),
                1000.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::FlowRate>(
                        value, Tucuxi::Common::TucuUnit("l/h"), Tucuxi::Common::TucuUnit("l/h")),
                1.0);
}

TEST (Common_Unit, TemperatureUnit){
    // clang-format off
    vector<string> temperatureUnits
        {
            "celsius"
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

    Tucuxi::Core::Value value = 1.0;

    for (const auto& unit : temperatureUnits) {
        ASSERT_TRUE(unitManager.isKnown(unit));
    }

    ASSERT_DOUBLE_EQ(
            unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::Temperature>(
                    value, Tucuxi::Common::TucuUnit("celsius"), Tucuxi::Common::TucuUnit("celsius")),
            1.0);

    ASSERT_DOUBLE_EQ(
            unitManager.convertToUnit(
                    value, Tucuxi::Common::TucuUnit("celsius"), Tucuxi::Common::TucuUnit("celsius")),
            1.0);
}

TEST (Common_Unit, NoUnit){
    // clang-format off
    vector<string> noUnit
        {
            "-",
            ""
        };
    // clang-format on

    Tucuxi::Common::UnitManager unitManager;

        Tucuxi::Core::Value value = 1.0;

        for (const auto& unit : noUnit) {
            ASSERT_TRUE(unitManager.isKnown(unit));
        }

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::NoUnit>(
                        value, Tucuxi::Common::TucuUnit(""), Tucuxi::Common::TucuUnit("")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::NoUnit>(
                        value, Tucuxi::Common::TucuUnit(""), Tucuxi::Common::TucuUnit("-")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::NoUnit>(
                        value, Tucuxi::Common::TucuUnit("-"), Tucuxi::Common::TucuUnit("")),
                1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit<Tucuxi::Common::UnitManager::UnitType::NoUnit>(
                        value, Tucuxi::Common::TucuUnit("-"), Tucuxi::Common::TucuUnit("-")),
                1.0);

        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit(value, Tucuxi::Common::TucuUnit(""), Tucuxi::Common::TucuUnit("")), 1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit(value, Tucuxi::Common::TucuUnit(""), Tucuxi::Common::TucuUnit("-")), 1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit(value, Tucuxi::Common::TucuUnit("-"), Tucuxi::Common::TucuUnit("")), 1.0);
        ASSERT_DOUBLE_EQ(
                unitManager.convertToUnit(value, Tucuxi::Common::TucuUnit("-"), Tucuxi::Common::TucuUnit("-")), 1.0);
}


