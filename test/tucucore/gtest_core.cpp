//@@license@@

#include <gtest/gtest.h>

#include "gtest_core.h"

int double_fuzzy_compare(double a, double b, double relative_tolerance, double absolute_tolerance)
{
    if (a == b) {
        return 0;
    }

    if (a == -b) {
        // Special case: Relative difference
        if (std::fabs(a - b) <= absolute_tolerance) {
            // is "infinite"
            // Fuzzy equality (via abs. tol. only)
            return 0;
        }
        else if (a > b) {
            return 1; // Fuzzy greater than
        }
        else {
            return -1; // Fuzzy less than
        }
    }

    const double diff = std::fabs(a - b);
    const double average = std::fabs((a + b) / 2.0);

    if (diff <= absolute_tolerance || diff / average <= relative_tolerance) {
        return 0; // Fuzzy equality.
    }
    else if (a > b) {
        return 1; // Fuzzy greater than
    }
    else {
        return -1; // Fuzzy less than
    }
}

bool double_eq_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance)
{
    return double_fuzzy_compare(a, b, relative_tolerance, absolute_tolerance) == 0;
}

bool double_ge_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance)
{
    return double_fuzzy_compare(a, b, relative_tolerance, absolute_tolerance) >= 0;
}

bool double_le_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance)
{
    return double_fuzzy_compare(a, b, relative_tolerance, absolute_tolerance) <= 0;
}

bool double_ne_rel_abs(double a, double b, double relative_tolerance, double absolute_tolerance)
{
    return double_fuzzy_compare(a, b, relative_tolerance, absolute_tolerance) != 0;
}

Tucuxi::Core::FormulationAndRoute getInfusionFormulationAndRoute()
{
    return Tucuxi::Core::FormulationAndRoute(
            Formulation::Test, AdministrationRoute::IntravenousDrip, AbsorptionModel::Infusion);
}

Tucuxi::Core::FormulationAndRoute getBolusFormulationAndRoute()
{
    return FormulationAndRoute(
            Formulation::Test, AdministrationRoute::IntravenousBolus, AbsorptionModel::Intravascular);
}

Tucuxi::Core::FormulationAndRoute getExtraFormulationAndRoute()
{
    return FormulationAndRoute(Formulation::Test, AdministrationRoute::Intramuscular, AbsorptionModel::Extravascular);
}

std::unique_ptr<DrugTreatment> buildDrugTreatment(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue,
        TucuUnit _unit,
        int interval,
        unsigned int nbrDoses)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, Duration(), Duration(std::chrono::hours(interval)));
    DosageRepeat repeatedDose(periodicDose, nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDateTime, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}


std::unique_ptr<DosageTimeRange> buildDosageTimeRange(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue,
        TucuUnit _unit,
        int interval,
        unsigned int nbrDoses)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, Duration(), Duration(std::chrono::hours(interval)));
    DosageRepeat repeatedDose(periodicDose, nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(startDateTime, repeatedDose);
    return dosageTimeRange;
}





int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
