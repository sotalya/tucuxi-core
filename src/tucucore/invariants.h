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


#ifndef TUCUXI_CORE_INVARIANTS_H
#define TUCUXI_CORE_INVARIANTS_H


namespace Tucuxi {
namespace Core {


enum class Invariants
{
    INV_0000 = 0,
    INV_ACTIVEMOIETY_0001,
    INV_ACTIVEMOIETY_0002,
    INV_ACTIVEMOIETY_0003,
    INV_ACTIVEMOIETY_0004,
    INV_ACTIVEMOIETY_0005,
    INV_ANALYTECONVERSION_0001,
    INV_ANALYTECONVERSION_0002,
    INV_ANALYTESETTOABSORPTIONASSOCIATION_0001,
    INV_ANALYTESETTOABSORPTIONASSOCIATION_0002,
    INV_ANALYTESET_0001,
    INV_ANALYTESET_0002,
    INV_ANALYTESET_0003,
    INV_ANALYTESET_0004,
    INV_ANALYTESET_0005,
    INV_ANALYTESET_0006,
    INV_ANALYTESET_0007,
    INV_ANALYTE_0001,
    INV_ANALYTE_0002,
    INV_ANALYTE_0003,
    INV_ANALYTE_0004,
    INV_CONSTRAINT_0001,
    INV_CONSTRAINT_0002,
    INV_CONSTRAINT_0003,
    INV_CONSTRAINT_0004,
    INV_CORRELATION_0001,
    INV_COVARIATEDEFINITION_0001,
    INV_DRUGMODEL_0001,
    INV_DRUGMODEL_0002,
    INV_DRUGMODEL_0003,
    INV_DRUGMODEL_0004,
    INV_DRUGMODEL_0005,
    INV_DRUGMODEL_0006,
    INV_DRUGMODEL_0007,
    INV_DRUGMODEL_0008,
    INV_DRUGMODEL_0009,
    INV_DRUGMODEL_0010,
    INV_DRUGMODEL_0011,
    INV_DRUGMODEL_0012,
    INV_DRUGMODEL_0013,
    INV_DRUGMODEL_0014,
    INV_DRUGMODEL_0015,
    INV_DRUGMODELDOMAIN_0001,
    INV_FORMULATIONANDROUTE_0001,
    INV_FORMULATIONANDROUTE_0002,
    INV_FORMULATIONANDROUTE_0003,
    INV_FULLFORMULATIONANDROUTE_0001,
    INV_FULLFORMULATIONANDROUTE_0002,
    INV_FULLFORMULATIONANDROUTE_0003,
    INV_FULLFORMULATIONANDROUTE_0004,
    INV_FULLFORMULATIONANDROUTE_0005,
    INV_FULLFORMULATIONANDROUTE_0006,
    INV_FULLFORMULATIONANDROUTE_0007,
    INV_FULLFORMULATIONANDROUTE_0008,
    INV_FULLFORMULATIONANDROUTE_0009,
    INV_FULLFORMULATIONANDROUTE_0010,
    INV_FULLFORMULATIONANDROUTE_0011,
    INV_FULLFORMULATIONANDROUTE_0012,
    INV_FULLFORMULATIONANDROUTE_0013,
    INV_HALFLIFE_0001,
    INV_HALFLIFE_0002,
    INV_INTERPARAMETERSETCORRELATION_0001,
    INV_INTERPARAMETERSETCORRELATION_0002,
    INV_PARAMETERDEFINITION_0001,
    INV_PARAMETERDEFINITION_0002,
    INV_PARAMETERDEFINITION_0003,
    INV_PARAMETERSETDEFINITION_0001,
    INV_PARAMETERSETDEFINITION_0002,
    INV_PARAMETERSETDEFINITION_0003,
    INV_STANDARDTREATMENT_0001,
    INV_STANDARDTREATMENT_0002,
    INV_TARGETDEFINITION_0001,
    INV_TARGETDEFINITION_0002,
    INV_TARGETDEFINITION_0003,
    INV_TARGETDEFINITION_0004,
    INV_TARGETDEFINITION_0005,
    INV_TARGETDEFINITION_0006,
    INV_TARGETDEFINITION_0007,
    INV_TARGETDEFINITION_0008,
    INV_TIMECONSIDERATIONS_0001,
    INV_TIMECONSIDERATIONS_0002,
    INV_TIMECONSIDERATIONS_0003,
    INV_TIMECONSIDERATIONS_0004,
    INV_VALIDDURATIONS_0001,
    INV_VALIDDURATIONS_0002,
    INV_VALIDDURATIONS_0003,
    INV_PARAMETERVARIABILITY_0001
};

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INVARIANT(invariant, expression, message)                                                              \
    {                                                                                                          \
        bool result = expression;                                                                              \
        if (!result) {                                                                                         \
            std::cout << "Drug model invariant failure : " << static_cast<int>(invariant) << ". " << (message) \
                      << std::endl;                                                                            \
        }                                                                                                      \
        ok &= result;                                                                                          \
        if (!ok) {                                                                                             \
            return false;                                                                                      \
        }                                                                                                      \
    }

/*
#define INVARIANT(invariant, expression) { \
    bool result = expression; if (!result) { \
    std::cout << "Invariant failure : " << \
    static_cast<int>(invariant) << \
    ". File " << __FILE__ << ", line " << __LINE__ << \
    std::endl;} \
    ok &= result; \
    if (!ok) {return false;} }
*/

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define COMPLEX_INVARIANT(invariant, expression) expression;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define INVARIANTS(decl)         \
public:                          \
    bool checkInvariants() const \
    {                            \
        bool ok = true;          \
        decl;                    \
        return ok;               \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define EMPTYINVARIANTS          \
public:                          \
    bool checkInvariants() const \
    {                            \
        return true;             \
    }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CHECKINVARIANTS checkInvariants()

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define LAMBDA_INVARIANT(invariant, expression, message)                                                           \
    {                                                                                                              \
        auto f = [this]() { expression };                                                                          \
        {                                                                                                          \
            bool result = f();                                                                                     \
            if (!result) {                                                                                         \
                std::cout << "Drug model invariant failure : " << static_cast<int>(invariant) << ". " << (message) \
                          << std::endl;                                                                            \
            }                                                                                                      \
            ok &= result;                                                                                          \
            if (!ok)                                                                                               \
                return false;                                                                                      \
        }                                                                                                          \
    }

/*
#define LAMBDA_INVARIANT(invariant, expression) {auto f=[this]() {expression}; {bool result = f();if (!result) { \
    std::cout << "Invariant failure : " << \
    static_cast<int>(invariant) << \
    ". File " << __FILE__ << ", line " << __LINE__ << \
    std::endl;} \
    ok &= result; if (!ok) return false;}}
*/
} // namespace Core
} // namespace Tucuxi

#endif // TUCUXI_CORE_INVARIANTS_H
