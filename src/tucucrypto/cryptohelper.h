//@@lisence@@

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
