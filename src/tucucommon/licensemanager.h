/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSEMANAGER_H
#define TUCUXI_TUCUCOMMON_LICENSEMANAGER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.

#include <string.h>

#include "tucucommon/systeminfo.h"

namespace Tucuxi {
namespace Common {

enum class LicenseError {
    VALID_LICENSE = 1,
    INVALID_LICENSE = 0,
    REQUEST_SUCCESSFUL = 0,
    INSTALLATION_SUCCESSFUL = 0,
    MISSING_LICENSE_FILE = -1,
    ERROR_CRYPTO = -2,
    NO_MACHINE_ID_FOUND = -3
};

struct MachineId {
    std::string m_fingerprint;
    MachineIdType m_type;
};

/// \ingroup LicenseManager
/// \brief Implement the licensing mechanism in the software.
///
/// A license is build with 5 fields : Key_word:Type:Fingerprint:End validity date:Last used date file
///
/// Key_word            : Allow to identify the requests of the licenses. (license)
/// Type                : Provided by enum class MachineIdType.
/// Fingerprint         : It is a unique id that identifies the machine.
/// End validity date   : Expiration date of the license.
/// Last used date file : This date is updated when the license file is checked.
///                       This prevents the user from artificially changing the date of his machine.
///
/// The license is given by the server.
///
/// A request to get a valid license is build with 4 fields : key_word:type:fingerprint:Date of request:version app
///
/// Key_word            : Allow to identify the requests of the licenses. (request)
/// Type                : Provided by enum class MachineIdType.
/// Fingerprint         : It is a unique id that identifies the machine.
/// Date of request     : Date of request.
/// Version app         : Version of Tucuxi.
///
/// The request is passed to server.
///
class LicenseManager
{
public:

    /// \brief Check if license file is valid.
    /// @param _filename : full path of license file.
    /// @return VALID_LICENSE or INVALID_LICENSE, In case of error : MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static int checkLicenseFile(std::string _filename);

    /// \brief Check if license is valid and install it in license file.
    /// @param _license  : encrypt string provided by the server.
    /// @param _filename : full path of license file.
    /// @return INSTALLATION_SUCCESSFUL, In case of error : INVALID_LICENSE, MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static int installLicense(std::string _license, std::string _filename);


    /// \brief Return a request to be sent to the server.
    /// @param _request : encrypt string.
    /// @return REQUEST_SUCCESSFUL, In case of error : NO_MACHINE_ID_FOUND, ERROR_CRYPTO.
    static int generateRequestString(std::string* _request);

private:

    /// \brief Check if license is valid.
    /// @param _request : encrypt string.
    /// @return VALID_LICENSE or INVALID_LICENSE, In case of error : ERROR_CRYPTO.
    static int checklicense(std::string _request);

    /// \brief Update the field of Last used date file.
    /// @param _license  : encrypt string provided by the server.
    /// @param _filename : full path of license file.
    /// @return INSTALLATION_SUCCESSFUL, In case of error : INVALID_LICENSE, MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static int rewriteLicense(std::string _license, std::string _filename);

private:
    static const std::string m_key;
};

}
}

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
