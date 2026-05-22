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

using namespace Tucuxi::Core;

int double_fuzzy_compare(double _a, double _b, double _relativeTolerance, double _absoluteTolerance)
{
    if (_a == _b) {
        return 0;
    }

    if (_a == -_b) {
        // Special case: Relative difference
        if (std::fabs(_a - _b) <= _absoluteTolerance) {
            // is "infinite"
            // Fuzzy equality (via abs. tol. only)
            return 0;
        }
        if (_a > _b) {
            return 1; // Fuzzy greater than
        }
        return -1; // Fuzzy less than
    }

    const double diff = std::fabs(_a - _b);
    const double average = std::fabs((_a + _b) / 2.0);

    if (diff <= _absoluteTolerance || diff / average <= _relativeTolerance) {
        return 0; // Fuzzy equality.
    }
    if (_a > _b) {
        return 1; // Fuzzy greater than
    }
    return -1; // Fuzzy less than
}

bool double_eq_rel_abs(double _a, double _b, double _relativeTolerance, double _absoluteTolerance)
{
    return double_fuzzy_compare(_a, _b, _relativeTolerance, _absoluteTolerance) == 0;
}

bool double_ge_rel_abs(double _a, double _b, double _relativeTolerance, double _absoluteTolerance)
{
    return double_fuzzy_compare(_a, _b, _relativeTolerance, _absoluteTolerance) >= 0;
}

bool double_le_rel_abs(double _a, double _b, double _relativeTolerance, double _absoluteTolerance)
{
    return double_fuzzy_compare(_a, _b, _relativeTolerance, _absoluteTolerance) <= 0;
}

bool double_ne_rel_abs(double _a, double _b, double _relativeTolerance, double _absoluteTolerance)
{
    return double_fuzzy_compare(_a, _b, _relativeTolerance, _absoluteTolerance) != 0;
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
        const DateTime& _startDateTime,
        DoseValue _doseValue,
        const TucuUnit& _unit,
        int _interval,
        unsigned int _nbrDoses,
        Duration _infusionTime)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, _infusionTime, Duration(std::chrono::hours(_interval)));
    DosageRepeat repeatedDose(periodicDose, _nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(_startDateTime, repeatedDose);

    drugTreatment->getModifiableDosageHistory().addTimeRange(*dosageTimeRange);

    return drugTreatment;
}


std::unique_ptr<DosageTimeRange> buildDosageTimeRange(
        const FormulationAndRoute& _route,
        const DateTime& _startDateTime,
        DoseValue _doseValue,
        const TucuUnit& _unit,
        int _interval,
        unsigned int _nbrDoses)
{
    auto drugTreatment = std::make_unique<DrugTreatment>();

    // List of time ranges that will be pushed into the history
    DosageTimeRangeList timeRangeList;

    LastingDose periodicDose(_doseValue, _unit, _route, Duration(), Duration(std::chrono::hours(_interval)));
    DosageRepeat repeatedDose(periodicDose, _nbrDoses);
    auto dosageTimeRange = std::make_unique<Tucuxi::Core::DosageTimeRange>(_startDateTime, repeatedDose);
    return dosageTimeRange;
}



#include "computingcomponentfactory.h"

int main(int argc, char** argv)
{
#ifdef TUCU_COMPILE_MULTI
    ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode::Comparator);
#else
    ComputingComponentFactory::setMode(ComputingComponentFactory::CreationMode::Single);
#endif // TUCU_COMPILE_MULTI
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
