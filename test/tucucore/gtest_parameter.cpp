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


#include <gtest/gtest.h>

#include "tucucore/drugmodel/parameterdefinition.h"
#include "tucucore/occasioniovcontext.h"
#include "tucucore/parameter.h"

#include "mocklogger.h"

using namespace Tucuxi::Core;

TEST(Core_TestParameter, ApplyEta)
{
    {
        // Test Proportional variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Proportional, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 15.0);
    }
    {
        // Test Exponential variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 5.0 * std::exp(2.0));
    }
    {
        // Test LogNormal variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::LogNormal, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 5.0 * std::exp(2.0));
    }
    {
        // Test Normal variability
        ParameterDefinition pDef(
                "pid", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Normal, 1.0));
        Parameter p(pDef, 5.0);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 7.0);
    }
    {
        // Test logit variability
        ParameterDefinition pDef(
                "pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
        Parameter p(pDef, 0.6);
        bool valid = p.applyEta(2.0);
        ASSERT_TRUE(valid);
        ASSERT_EQ(p.getValue(), 1.0 / (1.0 + std::exp(-(std::log(0.6 / (1.0 - 0.6)) + 2.0))));
    }
    {
        // Test logit variability with wrong parameter value

        Tucuxi::Common::ScopedMockLogger mockLogger;
        ParameterDefinition pDef(
                "pid", 0.5, std::make_unique<ParameterVariability>(ParameterVariabilityType::Logit, 1.0));
        Parameter p(pDef, 1.6);
        bool valid = p.applyEta(2.0);
        ASSERT_FALSE(valid);
        ASSERT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Warn, "makes it not a number"));
    }
}

TEST(Core_TestParameter, GetTime)
{
    ParameterSetSeries series;

    Tucuxi::Core::ParameterDefinitions parameterDefs;
    Tucuxi::Core::ParameterDefinition def("F", 100, Tucuxi::Core::ParameterVariabilityType::None);
    Tucuxi::Core::ParameterSetEvent parameters0(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(200))), parameterDefs);
    parameters0.addParameterEvent(def, 200);
    Tucuxi::Core::ParameterSetEvent parameters1(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(300))), parameterDefs);
    parameters1.addParameterEvent(def, 300);
    Tucuxi::Core::ParameterSetEvent parameters2(
            DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(400))), parameterDefs);
    parameters2.addParameterEvent(def, 400);
    Tucuxi::Core::ParameterSetSeries parametersSeries;
    parametersSeries.addParameterSetEvent(parameters0);
    parametersSeries.addParameterSetEvent(parameters1);
    parametersSeries.addParameterSetEvent(parameters2);
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(200))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 200);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(250))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 200);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(300))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 300);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(350))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 300);
        }
    }
    {
        auto ps = parametersSeries.getAtTime(
                DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(400))));
        for (auto p = ps->begin(); p < ps->end(); p++) {
            ASSERT_EQ(ps->getValue(ParameterId::F), 400);
        }
    }
}

// Helper building a DateTime a given number of minutes after the epoch.
static DateTime minutesAfterEpoch(int _minutes)
{
    return DateTime::fromDurationSinceEpoch(Tucuxi::Common::Duration(std::chrono::minutes(_minutes)));
}

// Eta-vector layout and time lookup.
TEST(Core_TestParameter, OccasionIovContextLookup)
{
    std::vector<IovOccasion> occasions;
    occasions.push_back(IovOccasion{minutesAfterEpoch(100), minutesAfterEpoch(200)});
    occasions.push_back(IovOccasion{minutesAfterEpoch(300), minutesAfterEpoch(400)});

    // Three variable parameters carry a BSV eta, and two of them also carry
    // an IOV. An IOV always implies a BSV, so the IOV etas per occasion can
    // never outnumber the BSV etas.
    OccasionIovContext ctx(3, 2, occasions);

    ASSERT_EQ(ctx.getBsvCount(), 3u);
    ASSERT_EQ(ctx.getIovCountPerOccasion(), 2u);
    ASSERT_EQ(ctx.getOccasionCount(), 2u);
    // Total = BSV head + one block per occasion = 3 + 2 * 2 = 7.
    ASSERT_EQ(ctx.getTotalEtaCount(), 7u);

    // Occasion blocks start right after the BSV head.
    ASSERT_EQ(ctx.occasionBlockOffset(0), 3u);
    ASSERT_EQ(ctx.occasionBlockOffset(1), 5u);

    // A time inside each occasion maps to its index.
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(150)), 0);
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(350)), 1);

    // Before, after, and in the gap: no occasion, hence no IOV.
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(50)), -1);
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(550)), -1);
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(250)), -1);

    // Start and end are both inclusive, so the end instant maps to the
    // occasion ending there (matches computeSampledOccasions).
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(100)), 0);
    ASSERT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(200)), 0);
}

// Apply, on top of the BSV eta, the IOV eta of the occasion that contains the
// requested time. Without a context it applies the BSV eta alone (prediction
// path).
TEST(Core_TestParameter, ApplyEtasWithIov)
{
    // Parameter CL with an exponential BSV and an IOV.
    ParameterDefinition clDef(
            "CL", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
    clDef.setIov(ParameterVariabilityType::Exponential, 0.5);

    // A single parameter set event, active from the epoch onwards.
    ParameterDefinitions emptyDefs;
    ParameterSetEvent event(minutesAfterEpoch(0), emptyDefs);
    event.addParameterEvent(clDef, 10.0);

    // Two back-to-back occasions: [100, 200) and [200, 300) minutes.
    std::vector<IovOccasion> occasions;
    occasions.push_back(IovOccasion{minutesAfterEpoch(100), minutesAfterEpoch(200)});
    occasions.push_back(IovOccasion{minutesAfterEpoch(200), minutesAfterEpoch(300)});
    auto context = std::make_shared<OccasionIovContext>(1, 1, occasions);

    ParameterSetSeries series;
    series.addParameterSetEvent(event);
    series.setIovContext(context);

    // Eta vector: BSV head then one IOV eta per occasion.
    //   index 0 = BSV, index 1 = occasion 0 IOV, index 2 = occasion 1 IOV.
    Etas etas = {0.1, 0.2, -0.3};

    // Before any occasion: only the BSV eta applies.
    {
        auto ps = series.getAtTime(minutesAfterEpoch(50), etas);
        ASSERT_NE(ps, nullptr);
        ASSERT_NEAR(ps->getValue(ParameterId::CL), 10.0 * std::exp(0.1), 1e-9);
    }
    // Inside occasion 0: BSV eta plus occasion 0 IOV eta.
    {
        auto ps = series.getAtTime(minutesAfterEpoch(150), etas);
        ASSERT_NE(ps, nullptr);
        ASSERT_NEAR(ps->getValue(ParameterId::CL), 10.0 * std::exp(0.1 + 0.2), 1e-9);
    }
    // Inside occasion 1: BSV eta plus occasion 1 IOV eta.
    {
        auto ps = series.getAtTime(minutesAfterEpoch(250), etas);
        ASSERT_NE(ps, nullptr);
        ASSERT_NEAR(ps->getValue(ParameterId::CL), 10.0 * std::exp(0.1 - 0.3), 1e-9);
    }

    // The same series with no context is the prediction path (BSV eta only,
    // even for a time that falls inside an occasion).
    {
        ParameterSetSeries predictionSeries;
        predictionSeries.addParameterSetEvent(event);
        Etas bsvOnly = {0.1};
        auto ps = predictionSeries.getAtTime(minutesAfterEpoch(250), bsvOnly);
        ASSERT_NE(ps, nullptr);
        ASSERT_NEAR(ps->getValue(ParameterId::CL), 10.0 * std::exp(0.1), 1e-9);
    }
}

// A time on a boundary shared by two occasions must map to the occasion ending
// there, the same occasion computeSampledOccasions assigns a sample on that
// boundary to. The two must never disagree.
TEST(Core_TestParameter, OccasionIndexAtTimeSharedBoundary)
{
    std::vector<IovOccasion> occasions = {
            {minutesAfterEpoch(100), minutesAfterEpoch(200)}, {minutesAfterEpoch(200), minutesAfterEpoch(300)}};
    OccasionIovContext ctx(1, 1, occasions);

    EXPECT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(150)), 0);
    EXPECT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(250)), 1);
    EXPECT_EQ(ctx.occasionIndexAtTime(minutesAfterEpoch(200)), 0);

    // computeSampledOccasions agrees: a sample at 200 belongs to occasion 0.
    std::vector<IovOccasion> sampled = computeSampledOccasions(occasions, {minutesAfterEpoch(200)});
    ASSERT_EQ(sampled.size(), static_cast<size_t>(1));
    EXPECT_EQ(sampled[0].m_start, minutesAfterEpoch(100));
}

// The IOV indices must form a compact [0, N) sequence in parameter order,
// skipping parameters that have no IOV, so they line up with the Omega
// IOV-block order the extractor builds.
TEST(Core_TestParameter, IovIndexAssignmentIsCompactAndOrdered)
{
    // Alphabetical order CL, Ka, V. CL and V carry an IOV, Ka does not.
    ParameterDefinition cl(
            "CL", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
    cl.setIov(ParameterVariabilityType::Exponential, 0.2);
    ParameterDefinition ka(
            "Ka", 1.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
    ParameterDefinition v(
            "V", 50.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
    v.setIov(ParameterVariabilityType::Exponential, 0.25);

    ParameterDefinitions emptyDefs;
    ParameterSetEvent event(minutesAfterEpoch(0), emptyDefs);
    event.addParameterEvent(cl, 10.0);
    event.addParameterEvent(ka, 1.0);
    event.addParameterEvent(v, 50.0);

    size_t expectedIovIndex = 0;
    bool checkedCl = false;
    bool checkedV = false;
    for (auto const& p : event) {
        if (p.getParameterId() == "Ka") {
            EXPECT_FALSE(p.getDefinition().hasIov());
            continue;
        }
        ASSERT_TRUE(p.getDefinition().hasIov());
        EXPECT_EQ(p.m_iovIndex, expectedIovIndex);
        checkedCl = checkedCl || (p.getParameterId() == "CL");
        checkedV = checkedV || (p.getParameterId() == "V");
        ++expectedIovIndex;
    }
    EXPECT_TRUE(checkedCl);
    EXPECT_TRUE(checkedV);
    EXPECT_EQ(expectedIovIndex, static_cast<size_t>(2));
}

// A sample on a dose boundary must be assigned to the interval that ends there,
// the same cycle the concentration calculator evaluates it in, not the next
// interval.
TEST(Core_TestParameter, SampledOccasionsBoundaryConvention)
{
    // Three back-to-back 6-hour intervals starting at the epoch.
    std::vector<IovOccasion> intervals = {
            {minutesAfterEpoch(0), minutesAfterEpoch(360)},
            {minutesAfterEpoch(360), minutesAfterEpoch(720)},
            {minutesAfterEpoch(720), minutesAfterEpoch(1080)}};

    // A sample exactly on the 360-minute boundary belongs to the first
    // interval, not the second.
    std::vector<IovOccasion> onBoundary = computeSampledOccasions(intervals, {minutesAfterEpoch(360)});
    ASSERT_EQ(onBoundary.size(), static_cast<size_t>(1));
    EXPECT_EQ(onBoundary[0].m_start, minutesAfterEpoch(0));

    // A sample strictly inside the second interval maps to the second
    // interval.
    std::vector<IovOccasion> inside = computeSampledOccasions(intervals, {minutesAfterEpoch(540)});
    ASSERT_EQ(inside.size(), static_cast<size_t>(1));
    EXPECT_EQ(inside[0].m_start, minutesAfterEpoch(360));

    // A sample at the treatment start maps to the first interval.
    std::vector<IovOccasion> atStart = computeSampledOccasions(intervals, {minutesAfterEpoch(0)});
    ASSERT_EQ(atStart.size(), static_cast<size_t>(1));
    EXPECT_EQ(atStart[0].m_start, minutesAfterEpoch(0));

    // Samples in two different intervals give two occasions, in time order.
    std::vector<IovOccasion> two = computeSampledOccasions(intervals, {minutesAfterEpoch(120), minutesAfterEpoch(800)});
    ASSERT_EQ(two.size(), static_cast<size_t>(2));
    EXPECT_EQ(two[0].m_start, minutesAfterEpoch(0));
    EXPECT_EQ(two[1].m_start, minutesAfterEpoch(720));
}

// A non-positive or absent IOV standard deviation must be ignored with a warning
// rather than reaching Omega.
TEST(Core_TestParameter, IovNonPositiveStdDevIsIgnored)
{
    // A zero IOV standard deviation is dropped with a warning, leaving no IOV.
    {
        Tucuxi::Common::ScopedMockLogger mockLogger;
        ParameterDefinition def(
                "CL", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
        def.setIov(ParameterVariabilityType::Exponential, 0.0);
        EXPECT_FALSE(def.hasIov());
        EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Warn, "non-positive IOV"));
    }
    // A negative IOV standard deviation is likewise dropped with a warning.
    {
        Tucuxi::Common::ScopedMockLogger mockLogger;
        ParameterDefinition def(
                "CL", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
        def.setIov(ParameterVariabilityType::Exponential, -0.5);
        EXPECT_FALSE(def.hasIov());
        EXPECT_TRUE(mockLogger.hasEntry(Tucuxi::Common::LogLevel::Warn, "non-positive IOV"));
    }
    // A positive IOV standard deviation is kept.
    {
        ParameterDefinition def(
                "CL", 10.0, std::make_unique<ParameterVariability>(ParameterVariabilityType::Exponential, 0.3));
        def.setIov(ParameterVariabilityType::Exponential, 0.5);
        ASSERT_TRUE(def.hasIov());
        ASSERT_EQ(def.getIov().getValues().size(), static_cast<size_t>(1));
        EXPECT_DOUBLE_EQ(def.getIov().getValue(), 0.5);
    }
}
