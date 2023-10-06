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


#ifndef GENERAL_H
#define GENERAL_H

//template <typename T>
//void ignore(T &&)
//{ }

/// This macro can be used when a function parameter is not to be used
/// Everytime we use it we should think about a potential refactoring
template<typename T>
void FINAL_UNUSED_PARAMETER(T&&)
{
}
// #define FINAL_UNUSED_PARAMETER(P) {ignore(P);}

/// This macro can be used when a function parameter is not yet used
/// but should be in the future.
///
/// At some stage the TMP_UNUSED_PARAMETER() should disappear
template<typename T>
void TMP_UNUSED_PARAMETER(T&&)
{
}
// #define TMP_UNUSED_PARAMETER(P) {ignore(P);}


// clang-format off
#ifndef TUCU_NOCATCH
#define TUCU_TRY try // NOLINT(cppcoreguidelines-macro-usage)
#define TUCU_CATCH(x) catch (x) // NOLINT(cppcoreguidelines-macro-usage)
#define TUCU_ONEXCEPTION(x) {x} // NOLINT(cppcoreguidelines-macro-usage)
#else
#define TUCU_TRY if (true) // NOLINT(cppcoreguidelines-macro-usage)
#define TUCU_CATCH(x) if (false) // NOLINT(cppcoreguidelines-macro-usage)
#define TUCU_ONEXCEPTION(x) {} // NOLINT(cppcoreguidelines-macro-usage)
#endif
// clang-format on

#endif // GENERAL_H
