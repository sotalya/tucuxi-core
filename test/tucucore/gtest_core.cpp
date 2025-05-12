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
    return Tucuxi::Core::FormulationAndRoute(Formulation::Test, AdministrationRoute::IntravenousDrip);
}
/*
Tucuxi::Core::AbsorptionModel getInfusionAbsorptionModel()
{
    return AbsorptionModel::Infusion;
}
*/
Tucuxi::Core::FormulationAndRoute getBolusFormulationAndRoute()
{
    return FormulationAndRoute(Formulation::Test, AdministrationRoute::IntravenousBolus);
}
/*
Tucuxi::Core::AbsorptionModel getBolusAbsorptionModel()
{
    return AbsorptionModel::Intravascular;
}
*/
Tucuxi::Core::FormulationAndRoute getExtraFormulationAndRoute()
{
    return FormulationAndRoute(Formulation::Test, AdministrationRoute::Intramuscular);
}
/*
Tucuxi::Core::AbsorptionModel getExtraAbsorptionModel()
{
    return AbsorptionModel::Extravascular;
}
*/
std::unique_ptr<DrugTreatment> buildDrugTreatment(
        const FormulationAndRoute& _route,
        const DateTime startDateTime,
        DoseValue _doseValue,
        TucuUnit _unit,
        int interval,
        unsigned int nbrDoses,
        Duration infusionTime)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, infusionTime, Duration(std::chrono::hours(interval)));
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
