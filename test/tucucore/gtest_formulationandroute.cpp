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


#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "tucucore/drugmodel/formulationandroute.h"

using namespace Tucuxi::Core;


// ============================================================
// FormulationAndRoute
// ============================================================

TEST(Core_TestFormulationAndRoute, DefaultConstruction)
{
    // Default-constructed object must expose Undefined for both enum fields and an empty name.
    FormulationAndRoute far;
    EXPECT_EQ(far.getFormulation(), Formulation::Undefined);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::Undefined);
    EXPECT_EQ(far.getAdministrationName(), "");
}

TEST(Core_TestFormulationAndRoute, ConstructionTwoParams)
{
    // Two-parameter constructor must store formulation and route; name must default to "".
    FormulationAndRoute far(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus);
    EXPECT_EQ(far.getFormulation(), Formulation::ParenteralSolution);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::IntravenousBolus);
    EXPECT_EQ(far.getAdministrationName(), "");
}

TEST(Core_TestFormulationAndRoute, ConstructionThreeParams)
{
    // Three-parameter constructor must store formulation, route, and administration name.
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Oral, "oral tablet");
    EXPECT_EQ(far.getFormulation(), Formulation::OralSolution);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::Oral);
    EXPECT_EQ(far.getAdministrationName(), "oral tablet");
}

TEST(Core_TestFormulationAndRoute, HasInfusionTrueForIntravenousDrip)
{
    // hasInfusion() must be true only when the route is IntravenousDrip.
    FormulationAndRoute far(Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip);
    EXPECT_TRUE(far.hasInfusion());
}

TEST(Core_TestFormulationAndRoute, HasInfusionFalseForIntravenousBolus)
{
    // A bolus route must not report an infusion.
    FormulationAndRoute far(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus);
    EXPECT_FALSE(far.hasInfusion());
}

TEST(Core_TestFormulationAndRoute, HasInfusionFalseForOral)
{
    // An oral route must not report an infusion.
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Oral);
    EXPECT_FALSE(far.hasInfusion());
}

TEST(Core_TestFormulationAndRoute, IsUndefinedWhenDefaultConstructed)
{
    // A default-constructed object is undefined (both fields are Undefined).
    FormulationAndRoute far;
    EXPECT_TRUE(far.isUndefined());
}

TEST(Core_TestFormulationAndRoute, IsUndefinedWhenOnlyFormulationIsUndefined)
{
    // isUndefined() must be true if the formulation is Undefined even if route is set.
    FormulationAndRoute far(Formulation::Undefined, AdministrationRoute::Oral);
    EXPECT_TRUE(far.isUndefined());
}

TEST(Core_TestFormulationAndRoute, IsUndefinedWhenOnlyRouteIsUndefined)
{
    // isUndefined() must be true if the route is Undefined even if formulation is set.
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Undefined);
    EXPECT_TRUE(far.isUndefined());
}

TEST(Core_TestFormulationAndRoute, IsNotUndefinedWhenBothSet)
{
    // isUndefined() must be false when both formulation and route are defined.
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Oral);
    EXPECT_FALSE(far.isUndefined());
}

TEST(Core_TestFormulationAndRoute, EqualityIdentical)
{
    // Two objects with the same formulation, route, and name must compare equal.
    FormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, "iv");
    FormulationAndRoute b(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, "iv");
    EXPECT_TRUE(a == b);
}

TEST(Core_TestFormulationAndRoute, InequalityDifferentFormulation)
{
    // Differing formulation must make objects unequal.
    FormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::Oral, "");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, "");
    EXPECT_FALSE(a == b);
}

TEST(Core_TestFormulationAndRoute, InequalityDifferentRoute)
{
    // Differing route must make objects unequal.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, "");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual, "");
    EXPECT_FALSE(a == b);
}

TEST(Core_TestFormulationAndRoute, InequalityDifferentName)
{
    // Differing administration name must make objects unequal.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, "tablet");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, "capsule");
    EXPECT_FALSE(a == b);
}

TEST(Core_TestFormulationAndRoute, IsCompatibleSameRoute)
{
    // isCompatible() depends only on route; differing formulation must not matter.
    FormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus);
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::IntravenousBolus);
    EXPECT_TRUE(a.isCompatible(b));
}

TEST(Core_TestFormulationAndRoute, IsNotCompatibleDifferentRoute)
{
    // Different routes must make objects incompatible.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral);
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual);
    EXPECT_FALSE(a.isCompatible(b));
}

TEST(Core_TestFormulationAndRoute, LessThanByFormulation)
{
    // ParenteralSolution (1) < OralSolution (2) in the Formulation enum.
    FormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::Oral, "");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, "");
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestFormulationAndRoute, LessThanByRouteWhenFormulationEqual)
{
    // Oral (5) < Sublingual (8) in the AdministrationRoute enum.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, "");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual, "");
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestFormulationAndRoute, LessThanByNameWhenFormulationAndRouteEqual)
{
    // Lexicographically smaller name must make the object strictly less than the other.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, "aaa");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, "bbb");
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestFormulationAndRoute, NotLessThanWhenEqual)
{
    // An object must not be less than an identical object.
    FormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, "tablet");
    FormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, "tablet");
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(b < a);
}


// ============================================================
// mergeFormulationAndRouteList (FormulationAndRoute overload)
// ============================================================

TEST(Core_TestFormulationAndRoute, MergeEmptyLists)
{
    // Merging two empty lists must produce an empty list.
    std::vector<FormulationAndRoute> v1;
    std::vector<FormulationAndRoute> v2;
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_TRUE(result.empty());
}

TEST(Core_TestFormulationAndRoute, MergeNonOverlappingLists)
{
    // Items with distinct routes from v2 must all be appended to v1.
    std::vector<FormulationAndRoute> v1 = {FormulationAndRoute(Formulation::OralSolution, AdministrationRoute::Oral)};
    std::vector<FormulationAndRoute> v2 = {
            FormulationAndRoute(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus)};
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 2u);
}

TEST(Core_TestFormulationAndRoute, MergeDeduplicatesCompatibleRoutes)
{
    // isCompatible() checks only the route, so two entries sharing the same route
    // are considered duplicates and the second one must not be added.
    std::vector<FormulationAndRoute> v1 = {
            FormulationAndRoute(Formulation::OralSolution, AdministrationRoute::Oral, "tablet")};
    std::vector<FormulationAndRoute> v2 = {
            FormulationAndRoute(Formulation::ParenteralSolution, AdministrationRoute::Oral, "syrup")};
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 1u);
}

TEST(Core_TestFormulationAndRoute, MergePreservesV1Order)
{
    // v1 content must appear first and unchanged in the merged result.
    std::vector<FormulationAndRoute> v1 = {FormulationAndRoute(Formulation::OralSolution, AdministrationRoute::Oral)};
    std::vector<FormulationAndRoute> v2;
    auto result = mergeFormulationAndRouteList(v1, v2);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAdministrationRoute(), AdministrationRoute::Oral);
}

TEST(Core_TestFormulationAndRoute, MergeWithEmptyV2)
{
    // Merging a non-empty v1 with an empty v2 must return a copy of v1.
    std::vector<FormulationAndRoute> v1 = {
            FormulationAndRoute(Formulation::OralSolution, AdministrationRoute::Oral),
            FormulationAndRoute(Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus)};
    std::vector<FormulationAndRoute> v2;
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 2u);
}


// ============================================================
// DMFormulationAndRoute
// ============================================================

TEST(Core_TestDMFormulationAndRoute, ConstructionWithAbsorptionModelOnly)
{
    // Single-parameter constructor must leave formulation and route as Undefined.
    DMFormulationAndRoute far(AbsorptionModel::Extravascular);
    EXPECT_EQ(far.getFormulation(), Formulation::Undefined);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::Undefined);
    EXPECT_EQ(far.getAbsorptionModel(), AbsorptionModel::Extravascular);
}

TEST(Core_TestDMFormulationAndRoute, ConstructionFourParams)
{
    // Full constructor must store all four fields.
    DMFormulationAndRoute far(
            Formulation::ParenteralSolution,
            AdministrationRoute::IntravenousBolus,
            AbsorptionModel::Intravascular,
            "iv bolus");
    EXPECT_EQ(far.getFormulation(), Formulation::ParenteralSolution);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::IntravenousBolus);
    EXPECT_EQ(far.getAbsorptionModel(), AbsorptionModel::Intravascular);
    EXPECT_EQ(far.getAdministrationName(), "iv bolus");
}

TEST(Core_TestDMFormulationAndRoute, ConstructionDefaultAdminName)
{
    // When no administration name is supplied the name must default to "".
    DMFormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    EXPECT_EQ(far.getAdministrationName(), "");
}

TEST(Core_TestDMFormulationAndRoute, HasInfusionTrueForIntravenousDrip)
{
    // hasInfusion() must be true only when the route is IntravenousDrip.
    DMFormulationAndRoute far(
            Formulation::ParenteralSolution, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
    EXPECT_TRUE(far.hasInfusion());
}

TEST(Core_TestDMFormulationAndRoute, HasInfusionFalseForBolus)
{
    // A bolus route must not report an infusion.
    DMFormulationAndRoute far(
            Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    EXPECT_FALSE(far.hasInfusion());
}

TEST(Core_TestDMFormulationAndRoute, EqualityIdentical)
{
    // Two objects with all four fields equal must compare equal.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "po");
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "po");
    EXPECT_TRUE(a == b);
}

TEST(Core_TestDMFormulationAndRoute, InequalityDifferentAbsorptionModel)
{
    // Differing absorption model must make objects unequal.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag);
    EXPECT_FALSE(a == b);
}

TEST(Core_TestDMFormulationAndRoute, InequalityDifferentRoute)
{
    // Differing route must make objects unequal.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual, AbsorptionModel::Extravascular);
    EXPECT_FALSE(a == b);
}

TEST(Core_TestDMFormulationAndRoute, InequalityDifferentFormulation)
{
    // Differing formulation must make objects unequal.
    DMFormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    EXPECT_FALSE(a == b);
}

TEST(Core_TestDMFormulationAndRoute, InequalityDifferentName)
{
    // Differing administration name must make objects unequal.
    DMFormulationAndRoute a(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "tab");
    DMFormulationAndRoute b(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "cap");
    EXPECT_FALSE(a == b);
}

TEST(Core_TestDMFormulationAndRoute, IsCompatibleWithSameModelAndRoute)
{
    // isCompatible(DM…) requires both absorptionModel and route to match.
    DMFormulationAndRoute a(
            Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    DMFormulationAndRoute b(
            Formulation::OralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    EXPECT_TRUE(a.isCompatible(b));
}

TEST(Core_TestDMFormulationAndRoute, IsNotCompatibleDifferentAbsorptionModel)
{
    // Different absorption models must make objects incompatible.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag);
    EXPECT_FALSE(a.isCompatible(b));
}

TEST(Core_TestDMFormulationAndRoute, IsNotCompatibleDifferentRoute)
{
    // Different routes must make objects incompatible even when model matches.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual, AbsorptionModel::Extravascular);
    EXPECT_FALSE(a.isCompatible(b));
}

TEST(Core_TestDMFormulationAndRoute, IsCompatibleWithFormulationAndRouteSameRoute)
{
    // isCompatible(FormulationAndRoute) checks only the route.
    DMFormulationAndRoute dm(
            Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::IntravenousBolus);
    EXPECT_TRUE(dm.isCompatible(far));
}

TEST(Core_TestDMFormulationAndRoute, IsNotCompatibleWithFormulationAndRouteDifferentRoute)
{
    // Different routes must make DM incompatible with a FormulationAndRoute.
    DMFormulationAndRoute dm(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    FormulationAndRoute far(Formulation::OralSolution, AdministrationRoute::Sublingual);
    EXPECT_FALSE(dm.isCompatible(far));
}

TEST(Core_TestDMFormulationAndRoute, GetTreatmentFormulationAndRoute)
{
    // getTreatmentFormulationAndRoute() must mirror formulation, route, and name.
    DMFormulationAndRoute dm(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "po");
    FormulationAndRoute far = dm.getTreatmentFormulationAndRoute();
    EXPECT_EQ(far.getFormulation(), Formulation::OralSolution);
    EXPECT_EQ(far.getAdministrationRoute(), AdministrationRoute::Oral);
    EXPECT_EQ(far.getAdministrationName(), "po");
}

TEST(Core_TestDMFormulationAndRoute, LessThanByFormulation)
{
    // ParenteralSolution (1) < OralSolution (2) in the Formulation enum.
    DMFormulationAndRoute a(Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestDMFormulationAndRoute, LessThanByRouteWhenFormulationEqual)
{
    // Oral (5) < Sublingual (8) in the AdministrationRoute enum.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Sublingual, AbsorptionModel::Extravascular);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestDMFormulationAndRoute, LessThanByAbsorptionModelWhenFormulationAndRouteEqual)
{
    // Extravascular (2) < ExtravascularLag (4) in the AbsorptionModel enum.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular);
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag);
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestDMFormulationAndRoute, LessThanByNameWhenAllOtherFieldsEqual)
{
    // Lexicographically smaller name must make the object strictly less.
    DMFormulationAndRoute a(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "aaa");
    DMFormulationAndRoute b(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "bbb");
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
}

TEST(Core_TestDMFormulationAndRoute, NotLessThanWhenEqual)
{
    // An object must not be less than an identical object.
    DMFormulationAndRoute a(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "po");
    DMFormulationAndRoute b(Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "po");
    EXPECT_FALSE(a < b);
    EXPECT_FALSE(b < a);
}


// ============================================================
// mergeFormulationAndRouteList (DMFormulationAndRoute overload)
// ============================================================

TEST(Core_TestDMFormulationAndRoute, MergeEmptyLists)
{
    // Merging two empty lists must produce an empty list.
    std::vector<DMFormulationAndRoute> v1;
    std::vector<DMFormulationAndRoute> v2;
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_TRUE(result.empty());
}

TEST(Core_TestDMFormulationAndRoute, MergeNonOverlappingLists)
{
    // Items with distinct (route, absorptionModel) pairs from v2 must all be appended.
    std::vector<DMFormulationAndRoute> v1 = {DMFormulationAndRoute(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular)};
    std::vector<DMFormulationAndRoute> v2 = {DMFormulationAndRoute(
            Formulation::ParenteralSolution, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular)};
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 2u);
}

TEST(Core_TestDMFormulationAndRoute, MergeDeduplicatesCompatibleEntries)
{
    // isCompatible(DM…) checks absorptionModel AND route, so two entries sharing both
    // are duplicates and the second must not be added.
    std::vector<DMFormulationAndRoute> v1 = {DMFormulationAndRoute(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "tab")};
    std::vector<DMFormulationAndRoute> v2 = {DMFormulationAndRoute(
            Formulation::ParenteralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular, "syrup")};
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 1u);
}

TEST(Core_TestDMFormulationAndRoute, MergeDifferentModelSameRouteBothKept)
{
    // Different absorption models with the same route are NOT compatible -> both kept.
    std::vector<DMFormulationAndRoute> v1 = {DMFormulationAndRoute(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular)};
    std::vector<DMFormulationAndRoute> v2 = {DMFormulationAndRoute(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::ExtravascularLag)};
    auto result = mergeFormulationAndRouteList(v1, v2);
    EXPECT_EQ(result.size(), 2u);
}

TEST(Core_TestDMFormulationAndRoute, MergePreservesV1Content)
{
    // The merged result must begin with v1's content.
    std::vector<DMFormulationAndRoute> v1 = {DMFormulationAndRoute(
            Formulation::OralSolution, AdministrationRoute::Oral, AbsorptionModel::Extravascular)};
    std::vector<DMFormulationAndRoute> v2;
    auto result = mergeFormulationAndRouteList(v1, v2);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAdministrationRoute(), AdministrationRoute::Oral);
    EXPECT_EQ(result[0].getAbsorptionModel(), AbsorptionModel::Extravascular);
}


// ============================================================
// StandardTreatment
// ============================================================

TEST(Core_TestStandardTreatment, DefaultConstruction)
{
    // Default-constructed object must not be a fixed duration and must have zero duration.
    StandardTreatment st;
    EXPECT_FALSE(st.getIsFixedDuration());
    EXPECT_DOUBLE_EQ(st.getDuration(), 0.0);
}

TEST(Core_TestStandardTreatment, ConstructionWithFixedDuration)
{
    // Fixed-duration constructor must store all three fields.
    StandardTreatment st(true, 7.0, TucuUnit("h"));
    EXPECT_TRUE(st.getIsFixedDuration());
    EXPECT_DOUBLE_EQ(st.getDuration(), 7.0);
    EXPECT_EQ(st.getUnit().toString(), "h");
}

TEST(Core_TestStandardTreatment, ConstructionNotFixed)
{
    // When isFixedDuration is false, the getter must reflect that.
    StandardTreatment st(false, 0.0, TucuUnit("h"));
    EXPECT_FALSE(st.getIsFixedDuration());
}

TEST(Core_TestStandardTreatment, DurationInDays)
{
    // The unit field must store whatever unit is provided.
    StandardTreatment st(true, 14.0, TucuUnit("d"));
    EXPECT_DOUBLE_EQ(st.getDuration(), 14.0);
    EXPECT_EQ(st.getUnit().toString(), "d");
}


// ============================================================
// AnalyteConversion
// ============================================================

TEST(Core_TestAnalyteConversion, ConstructionAndGetters)
{
    // Constructor must store both the analyte id and the conversion factor.
    AnalyteConversion conv(AnalyteId("drug_A"), 2.5);
    EXPECT_EQ(conv.getAnalyteId(), AnalyteId("drug_A"));
    EXPECT_DOUBLE_EQ(conv.getFactor(), 2.5);
}

TEST(Core_TestAnalyteConversion, ZeroFactor)
{
    // A conversion factor of zero must be stored and returned as-is.
    AnalyteConversion conv(AnalyteId("metabolite"), 0.0);
    EXPECT_DOUBLE_EQ(conv.getFactor(), 0.0);
}

TEST(Core_TestAnalyteConversion, UnitFactor)
{
    // A conversion factor of 1.0 (identity) must be stored correctly.
    AnalyteConversion conv(AnalyteId("active_drug"), 1.0);
    EXPECT_DOUBLE_EQ(conv.getFactor(), 1.0);
}

TEST(Core_TestAnalyteConversion, LargeFactor)
{
    // A large conversion factor must be stored without loss.
    AnalyteConversion conv(AnalyteId("drug"), 1000.0);
    EXPECT_DOUBLE_EQ(conv.getFactor(), 1000.0);
}

TEST(Core_TestAnalyteConversion, AnalyteIdIsStored)
{
    // The analyte id must be retrievable exactly as supplied.
    AnalyteConversion conv(AnalyteId("imatinib_active"), 0.5);
    EXPECT_EQ(conv.getAnalyteId(), AnalyteId("imatinib_active"));
}
