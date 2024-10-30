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


#ifndef TUCUXI_TUCUCOMMON_CRYPTOHELPER_H
#define TUCUXI_TUCUCOMMON_CRYPTOHELPER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <string>

namespace Tucuxi {
namespace Common {

/// \ingroup TucuCommon
/// \brief Helper class to simplify cryptographic operation.
///
/// This is a wrapper around the botan library. This class provide methods to hash, encrypt or decrypt messages.
///
/// An additional function let to generate a private symmetric key. This function is not used in application because
/// the key must be the same for the application and the server.
///
class CryptoHelper
{
public:
    static bool generateKey(std::string& _key);
    static bool hash(const std::string& _plaintext, std::string& _result);
    static bool encrypt(const std::string& _key, const std::string& _plaintext, std::string& _result);
    static bool decrypt(const std::string& _key, const std::string& _ciphertextiv, std::string& _result);
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
