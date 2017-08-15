/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSEGENERATOR_H
#define TUCUXI_TUCUCOMMON_LICENSEGENERATOR_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <string.h>

#include "tucucommon/systeminfo.h"
#include "tucucommon/datetime.h"

namespace Tucuxi {
namespace License {

enum class RequestError {
    REQUEST_SUCCESSFUL = 0,
    INVALID_LICENSE = -1,
    ERROR_CRYPTO = -2
};

struct Request {
    std::string m_keyWord;              // Allow to identify the requests of the licenses. (request)
    int m_type;                         // Provided by enum class MachineIdType.
    std::string m_fingerprint;          // It is a unique id that identifies the machine.
    Tucuxi::Common::DateTime m_date;    // Date of request.
    std::string m_version;              // Version of Tucuxi.
};

/// \ingroup LicenseGenerator
/// \brief ...
///
///
class LicenseGenerator
{
public:

    /// \brief Decrypt and return plain request.
    /// @param _encryptedRequest : encrypted request from application.
    /// @param _plainRequest : request from application.
    /// @return 0 in case of Success, -1 otherwise.
    static int decryptRequest(std::string _encryptedRequest, Request* _plainRequest);

    /// \brief Return a license to be sent to the application.
    /// @param _request : request from application.
    /// @return license, in case of error : an empty string.
    static std::string generateLicense(Request _request);



private:
    static const std::string m_key;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
