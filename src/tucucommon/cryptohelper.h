/*
 * Copyright (C) 2017 Tucuxi SA
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

class CryptoHelper
{
public:
    static bool generateKey(std::string* _key);
    static bool hash(std::string plaintext, std::string* result);
    static bool encrypt(std::string _key, std::string plaintext, std::string* result);
    static bool decrypt(std::string _key, std::string cyphertext, std::string* result);
};

}
}

#endif // TUCUXI_TUCUCOMMON_LOGGER_H
