/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <memory>

#include <gtest/gtest.h>

#include "tucucommon/duration.h"

#include "tucucore/definitions.h"
#include "tucucore/drugtreatment/target.h"

using namespace Tucuxi::Core;
using Tucuxi::Common::Duration;


// ============================================================
// Constructor 1: Target(id, type, min, best, max)
// ============================================================

TEST(Core_TestTarget, Ctor1BasicGetters)
{
    // All three concentration limits and the identity fields must be stored.
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 2.0, 5.0, 10.0);

    EXPECT_EQ(t.getActiveMoietyId(), ActiveMoietyId("drug"));
    EXPECT_EQ(t.getTargetType(), TargetType::Residual);
    EXPECT_DOUBLE_EQ(t.getValueMin(), 2.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 5.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 10.0);
}

TEST(Core_TestTarget, Ctor1DefaultMicIsZero)
{
    // Constructor 1 does not accept a MIC: getMicValue() must be 0.
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 1.0, 3.0, 6.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.0);
}

TEST(Core_TestTarget, Ctor1DefaultAlarmsAreZero)
{
    // Constructor 1 does not accept alarm values: both must default to 0.
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 1.0, 3.0, 6.0);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 0.0);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 0.0);
}


// ============================================================
// Constructor 2: Target(id, type, min, best, max, inefficacyAlarm, toxicityAlarm)
// ============================================================

TEST(Core_TestTarget, Ctor2Alarms)
{
    // Both alarm values must be stored correctly.
    Target t(ActiveMoietyId("drug"), TargetType::Peak, 5.0, 15.0, 25.0, 1.0, 30.0);

    EXPECT_DOUBLE_EQ(t.getValueMin(), 5.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 15.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 25.0);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 1.0);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 30.0);
}

TEST(Core_TestTarget, Ctor2DefaultMicIsZero)
{
    // Constructor 2 does not accept a MIC: getMicValue() must be 0.
    Target t(ActiveMoietyId("drug"), TargetType::Peak, 5.0, 15.0, 25.0, 1.0, 30.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.0);
}


// ============================================================
// Constructor 3: Target(id, type, unit, min, best, max, inefficacyAlarm, toxicityAlarm)
// ============================================================

TEST(Core_TestTarget, Ctor3WithUnit)
{
    // The explicitly provided unit must be stored and returned.
    TucuUnit unit("mg/l");
    Target t(ActiveMoietyId("drug"), TargetType::Mean, unit, 3.0, 7.0, 12.0, 0.5, 15.0);

    EXPECT_EQ(t.getUnit().toString(), unit.toString());
    EXPECT_DOUBLE_EQ(t.getValueMin(), 3.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 7.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 12.0);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 0.5);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 15.0);
}

TEST(Core_TestTarget, Ctor3DefaultMicIsZero)
{
    // Constructor 3 does not accept a MIC: getMicValue() must be 0.
    Target t(ActiveMoietyId("drug"), TargetType::Mean, TucuUnit("mg/l"), 3.0, 7.0, 12.0, 0.5, 15.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.0);
}


// ============================================================
// Constructor 4: Target(id, type, min, best, max, mic)
// ============================================================

TEST(Core_TestTarget, Ctor4WithMic)
{
    // The MIC value must be stored.
    Target t(ActiveMoietyId("antibiotic"), TargetType::AucOverMic, 1.0, 4.0, 8.0, 0.25);

    EXPECT_DOUBLE_EQ(t.getValueMin(), 1.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 4.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 8.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.25);
}

TEST(Core_TestTarget, Ctor4DefaultAlarmsAreZero)
{
    // Constructor 4 does not accept alarm values: both must default to 0.
    Target t(ActiveMoietyId("antibiotic"), TargetType::AucOverMic, 1.0, 4.0, 8.0, 0.25);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 0.0);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 0.0);
}


// ============================================================
// Constructor 5: Target(id, type, unit, min, best, max, mic, micUnit, tmin, tbest, tmax)
// ============================================================

TEST(Core_TestTarget, Ctor5WithMicAndTimings)
{
    // The unit, concentration limits, MIC value and MIC unit must all be stored.
    TucuUnit unit("mg/l");
    TucuUnit micUnit("mg/l");
    Duration tmin(std::chrono::minutes(30));
    Duration tbest(std::chrono::hours(1));
    Duration tmax(std::chrono::hours(2));

    Target t(ActiveMoietyId("drug"), TargetType::Peak, unit, 10.0, 20.0, 30.0, 0.5, micUnit, tmin, tbest, tmax);

    EXPECT_EQ(t.getUnit().toString(), unit.toString());
    EXPECT_DOUBLE_EQ(t.getValueMin(), 10.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 20.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 30.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.5);
    EXPECT_EQ(t.getMicUnit().toString(), micUnit.toString());
}


// ============================================================
// Constructor 6: Target(id, type, unit, min, best, max, inefficacyAlarm,
//                       toxicityAlarm, mic, micUnit)
// ============================================================

TEST(Core_TestTarget, Ctor6AlarmsAndMic)
{
    // All alarm, MIC, and unit values must be stored.
    TucuUnit unit("mg/l");
    TucuUnit micUnit("mg/l");

    Target t(ActiveMoietyId("antibiotic"), TargetType::TimeOverMic, unit, 5.0, 10.0, 20.0, 1.0, 25.0, 0.5, micUnit);

    EXPECT_EQ(t.getUnit().toString(), unit.toString());
    EXPECT_DOUBLE_EQ(t.getValueMin(), 5.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 10.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 20.0);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 1.0);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 25.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.5);
    EXPECT_EQ(t.getMicUnit().toString(), micUnit.toString());
}


// ============================================================
// Constructor 7: Target(id, type, unit, min, best, max, inefficacyAlarm,
//                       toxicityAlarm, mic, micUnit, tmin, tbest, tmax)
// ============================================================

TEST(Core_TestTarget, Ctor7AllParameters)
{
    // Every accessible field must be stored correctly.
    TucuUnit unit("mg/l");
    TucuUnit micUnit("mg/l");
    Duration tmin(std::chrono::minutes(30));
    Duration tbest(std::chrono::hours(1));
    Duration tmax(std::chrono::hours(2));

    Target t(
            ActiveMoietyId("antibiotic"),
            TargetType::FractionTimeOverMic,
            unit,
            2.0,
            6.0,
            12.0,
            0.5,
            15.0,
            0.25,
            micUnit,
            tmin,
            tbest,
            tmax);

    EXPECT_EQ(t.getActiveMoietyId(), ActiveMoietyId("antibiotic"));
    EXPECT_EQ(t.getTargetType(), TargetType::FractionTimeOverMic);
    EXPECT_EQ(t.getUnit().toString(), unit.toString());
    EXPECT_DOUBLE_EQ(t.getValueMin(), 2.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 6.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 12.0);
    EXPECT_DOUBLE_EQ(t.getInefficacyAlarm(), 0.5);
    EXPECT_DOUBLE_EQ(t.getToxicityAlarm(), 15.0);
    EXPECT_DOUBLE_EQ(t.getMicValue(), 0.25);
    EXPECT_EQ(t.getMicUnit().toString(), micUnit.toString());
}


// ============================================================
// TargetType variants
// ============================================================

TEST(Core_TestTarget, TargetTypeResidual)
{
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 1.0, 3.0, 5.0);
    EXPECT_EQ(t.getTargetType(), TargetType::Residual);
}

TEST(Core_TestTarget, TargetTypePeak)
{
    Target t(ActiveMoietyId("drug"), TargetType::Peak, 10.0, 15.0, 20.0);
    EXPECT_EQ(t.getTargetType(), TargetType::Peak);
}

TEST(Core_TestTarget, TargetTypeMean)
{
    Target t(ActiveMoietyId("drug"), TargetType::Mean, 3.0, 6.0, 9.0);
    EXPECT_EQ(t.getTargetType(), TargetType::Mean);
}

TEST(Core_TestTarget, TargetTypeAuc)
{
    Target t(ActiveMoietyId("drug"), TargetType::Auc, 100.0, 200.0, 400.0);
    EXPECT_EQ(t.getTargetType(), TargetType::Auc);
}

TEST(Core_TestTarget, TargetTypeAuc24)
{
    Target t(ActiveMoietyId("drug"), TargetType::Auc24, 100.0, 200.0, 400.0);
    EXPECT_EQ(t.getTargetType(), TargetType::Auc24);
}

TEST(Core_TestTarget, TargetTypeAucOverMic)
{
    Target t(ActiveMoietyId("drug"), TargetType::AucOverMic, 50.0, 100.0, 200.0, 0.5);
    EXPECT_EQ(t.getTargetType(), TargetType::AucOverMic);
}

TEST(Core_TestTarget, TargetTypeTimeOverMic)
{
    Target t(ActiveMoietyId("drug"), TargetType::TimeOverMic, 0.4, 0.6, 0.8, 0.5);
    EXPECT_EQ(t.getTargetType(), TargetType::TimeOverMic);
}


// ============================================================
// ActiveMoietyId
// ============================================================

TEST(Core_TestTarget, ActiveMoietyIdIsStored)
{
    // getActiveMoietyId() must return the exact id passed to the constructor.
    Target t(ActiveMoietyId("ibuprofen"), TargetType::Residual, 1.0, 3.0, 5.0);
    EXPECT_EQ(t.getActiveMoietyId(), ActiveMoietyId("ibuprofen"));
}

TEST(Core_TestTarget, DifferentActiveMoietyIds)
{
    // Two targets with different moiety ids must return different ids.
    Target t1(ActiveMoietyId("drugA"), TargetType::Residual, 1.0, 3.0, 5.0);
    Target t2(ActiveMoietyId("drugB"), TargetType::Residual, 1.0, 3.0, 5.0);
    EXPECT_NE(t1.getActiveMoietyId(), t2.getActiveMoietyId());
}


// ============================================================
// Value ordering: min < best < max
// ============================================================

TEST(Core_TestTarget, ValuesAreDistinct)
{
    // min, best, and max must be stored independently and remain distinct.
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 1.0, 5.0, 10.0);
    EXPECT_LT(t.getValueMin(), t.getValueBest());
    EXPECT_LT(t.getValueBest(), t.getValueMax());
}

TEST(Core_TestTarget, ValuesCanBeEqual)
{
    // The class does not enforce min < best < max; equal values are allowed.
    Target t(ActiveMoietyId("drug"), TargetType::Residual, 5.0, 5.0, 5.0);
    EXPECT_DOUBLE_EQ(t.getValueMin(), 5.0);
    EXPECT_DOUBLE_EQ(t.getValueBest(), 5.0);
    EXPECT_DOUBLE_EQ(t.getValueMax(), 5.0);
}


// ============================================================
// MIC unit
// ============================================================

TEST(Core_TestTarget, MicUnitIsStored)
{
    // getMicUnit() must return the unit passed to the constructor.
    TucuUnit unit("mg/l");
    TucuUnit micUnit("mg/l");
    Duration d(std::chrono::hours(1));

    Target t(ActiveMoietyId("drug"), TargetType::Peak, unit, 5.0, 10.0, 20.0, 0.5, micUnit, d, d, d);

    EXPECT_EQ(t.getMicUnit().toString(), micUnit.toString());
}
