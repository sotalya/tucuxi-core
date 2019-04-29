/*
 * Copyright (C) 2017 Tucuxi SA
 */

#ifndef TUCUXI_TUCUCOMMON_LICENSEMANAGER_H
#define TUCUXI_TUCUCOMMON_LICENSEMANAGER_H

/// \defgroup TucuCommon Tucuxi common library
/// \brief Common classes and components for Tucuxi applications
/// This module defines a set of basic and application independent classes and components for
/// use in other Tucuxi's libraries and applications.


#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/systeminfo.h"

namespace Tucuxi {
namespace Common {

enum class LicenseError {
    INVALID_LICENSE = 0,
    VALID_LICENSE = 1,
    REQUEST_SUCCESSFUL = 1,
    INSTALLATION_SUCCESSFUL = 1,
    MISSING_LICENSE_FILE = -1,
    ERROR_CRYPTO = -2,
    FINGERPRINT_ERROR = -3
};

enum class LicenseRequestError {
    REQUEST_SUCCESSFUL = 0,
    INVALID_REQUEST = -1,
    ERROR_CRYPTO = -2
};

class StringBasedData
{
public:
    StringBasedData() {}
    const std::string& getKeyword() { return m_keyword; }

protected:
    bool extract(std::string& _license, std::string &_value) const;
    bool extract(std::string& _license, int &_value) const;

protected:
    std::string m_keyword;
};

class LicenseRequest : public StringBasedData
{
public:
    LicenseRequest() {}
    LicenseRequest(MachineIdType _type, const std::string& _hashedFingerprint, const DateTime& _date, const std::string& _version);

public:
    bool fromString(const std::string& _strLisence);
    std::string toString() const;

public:
    MachineIdType getIdType() const                     { return static_cast<MachineIdType>(m_type); }
    const std::string& getHashedFingerprint() const     { return m_hashedFingerprint; }
    int getDate() const                                 { return m_date; }
    const std::string& getVersion() const               { return m_version; }

private:
    int m_type;                         // Provided by enum class MachineIdType.
    std::string m_hashedFingerprint;    // It is a unique id that identifies the machine.
    int m_date;                         // Date of request.
    std::string m_version;              // Version of Tucuxi.
};

class License : public StringBasedData
{
public:
    License() {}
    License(const LicenseRequest& _request, const DateTime& _endDate);

public:
    bool fromString(const std::string& _strLisence);
    std::string toString() const;

    void update();

    MachineIdType getIdType() const                     { return static_cast<MachineIdType>(m_type); }
    const std::string& getHashedFingerprint() const     { return m_hashedFingerprint; }
    int getValidityDate() const                         { return m_validityDate; }
    int getLastUsedDate() const                         { return m_lastUsedDate; }

private:
    int m_type;                       // Provided by enum class MachineIdType
    std::string m_hashedFingerprint;  // It is a unique id that identifies the machine
    int m_validityDate;               // Date of validity
    int m_lastUsedDate;               // Date of the last use
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
// -------------------------------------------------------
// Methods used by the application
public: 
    /// \brief Check if license file is valid.
    /// \param _filename : full path of license file.
    /// \return VALID_LICENSE or INVALID_LICENSE, In case of error : MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static LicenseError checkLicenseFile(const std::string &_filename);

    /// \brief Check if license is valid and install it in license file.
    /// \param _license  : encrypt string provided by the server.
    /// \param _filename : full path of license file.
    /// \return INSTALLATION_SUCCESSFUL, In case of error : INVALID_LICENSE, MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static LicenseError installLicense(const std::string &_license, const std::string &_filename);


    /// \brief Return a request to be sent to the server.
    /// \param _request : encrypt string.
    /// \return REQUEST_SUCCESSFUL, In case of error : NO_MACHINE_ID_FOUND, ERROR_CRYPTO.
    static LicenseError generateRequestString(std::string& _request, const std::string& _version);


    /// \brief Check if license is valid.
    /// \param _cryptedLicense : encrypt string.
    /// \return VALID_LICENSE or INVALID_LICENSE, In case of error : ERROR_CRYPTO.
    static LicenseError checklicense(const std::string &_cryptedLicense);

// -------------------------------------------------------
// Methods used by the server
public: 
    /// \brief Decrypt and return plain request.
    /// @param _encryptedRequest : encrypted request from application.
    /// @param _plainRequest : request from application.
    /// @return 0 in case of Success, -1 otherwise.
    static LicenseRequestError decryptRequest(const std::string &_encryptedRequest, LicenseRequest& _request);

    /// \brief Return a license to be sent to the application.
    /// @param _request : request from application.
    /// @return license, in case of error : an empty string.
    static LicenseRequestError generateLicense(const LicenseRequest &_request, std::string& _license, const Duration& _duration);


private:
    /// \brief Update the field of Last used date file.
    /// \param _cryptedLicense  : encrypted string provided by the server.
    /// \param _filename : full path of license file.
    /// \return INSTALLATION_SUCCESSFUL, In case of error : INVALID_LICENSE, MISSING_LICENSE_FILE, ERROR_CRYPTO.
    static LicenseError rewriteLicense(const std::string &_cryptedLicense, const std::string &_filename);


    static bool getHashedFingerprint(MachineIdType &_idType, std::string& _hashedFingerprint);

private:
    static const std::string sm_key; // NOLINT(readability-identifier-naming)
};

} // namespace Common
} // namespace Tucuxi

#endif // TUCUXI_TUCUCOMMON_LICENSECHECKER_H
