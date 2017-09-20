/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/utils.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/licensemanager.h"
#include "tucucommon/datetime.h"
#include "tucucommon/duration.h"
#include "tucucommon/cryptohelper.h"

using namespace std::chrono_literals;

namespace Tucuxi {
namespace Common {

// Symmetric key between Tucuxi and Server
const std::string LicenseManager::m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

static const std::string LICENSE_KEYWORD = "license";
static const std::string LICENSEREQUEST_KEYWORD = "request";

int dateToInt(const DateTime& _date) {
    char strDate[9];
    sprintf_s(strDate, "%i%02i%02i", _date.year(), _date.month(), _date.day());
    try {
        return std::stoi(strDate);
    }
    catch (...) {
        return -1;
    }
}

bool StringBasedData::extract(std::string& _license, std::string &_value) const
{
    bool isLastValue = false;
    std::size_t posMarker = _license.find(":");
    if (posMarker == std::string::npos) {
        if (_license.size() == 0) {
            return false;
        }
        posMarker = _license.size();
        isLastValue = true;
    }
    _value = _license.substr(0, posMarker);
    _license = isLastValue ? "" : _license.substr(posMarker + 1, std::string::npos);
    return true;
}

bool StringBasedData::extract(std::string& _license, int &_value) const
{
    std::string strValue;
    if (!extract(_license, strValue)) {
        return false;
    }
    try {
        _value = stoi(strValue);
    }
    catch (...) {
        return false;
    }
    return true;
}

LicenseRequest::LicenseRequest(MachineIdType _type, const std::string& _hashedFingerprint, const DateTime& _date, const std::string& _version) :    
    m_type(static_cast<int>(_type)),
    m_hashedFingerprint(_hashedFingerprint),
    m_version(_version)
{
    m_keyword = LICENSEREQUEST_KEYWORD;
    m_date = dateToInt(_date);
}

bool LicenseRequest::fromString(const std::string& _strLisence)
{
    std::string buffer = _strLisence;

    // Read request : key_word:type:fingerprint:request date:version app
    bool bOk = extract(buffer, m_keyword);
    bOk &= extract(buffer, m_type);
    bOk &= extract(buffer, m_hashedFingerprint);
    bOk &= extract(buffer, m_date);
    bOk &= extract(buffer, m_version);
    return bOk;
}

std::string LicenseRequest::toString() const
{
    return Utils::strFormat(
        "%s:%i:%s:%i:%s",
        m_keyword.c_str(),
        m_type,
        m_hashedFingerprint.c_str(),
        m_date,
        m_version.c_str());
}

License::License(const LicenseRequest& _request, const DateTime& _endDate) :
    m_type(static_cast<int>(_request.getIdType())),
    m_hashedFingerprint(_request.getHashedFingerprint())
{
    m_keyword = LICENSE_KEYWORD;
    m_validityDate = dateToInt(_endDate);
    m_lastUsedDate = dateToInt(DateTime());
}

bool License::fromString(const std::string& _strLisence)
{
    std::string buffer = _strLisence;

    // Read license : license:0:ABCDABCDABCDABCDABCDABCD:yyyy/mm/dd:yyyy/mm/dd
    // Read license : key_word:type:fingerprint:end validity date:last used date
    bool bOk = extract(buffer, m_keyword);
    bOk &= extract(buffer, m_type);
    bOk &= extract(buffer, m_hashedFingerprint);
    bOk &= extract(buffer, m_validityDate);
    bOk &= extract(buffer, m_lastUsedDate);
    return bOk;
}

std::string License::toString() const
{
    return Utils::strFormat(
        "%s:%i:%s:%i:%i",
        m_keyword.c_str(),
        m_type,
        m_hashedFingerprint.c_str(),
        m_validityDate,
        m_lastUsedDate);
}

void License::update()
{
    m_lastUsedDate = dateToInt(DateTime());
}

LicenseError LicenseManager::checkLicenseFile(const std::string &_filename)
{
    // Read content of license file
    std::string license;
    std::ifstream licenseFile(_filename);
    if (!licenseFile.is_open()) {
        LoggerHelper logger;
        logger.error("License file not found.");
        return LicenseError::MISSING_LICENSE_FILE;
    }    
    std::getline(licenseFile, license);
    licenseFile.close();

    // Check license file
    LicenseError res = checklicense(license);
    if (res != LicenseError::VALID_LICENSE) {
        return res;
    }

    // Update last used date in license file
    res = rewriteLicense(license, _filename);
    if (res == LicenseError::INSTALLATION_SUCCESSFUL) {
        res = LicenseError::VALID_LICENSE;
    }

    return res;
}


LicenseError LicenseManager::checklicense(const std::string &_cryptedLicense)
{    
    LoggerHelper logger;

    // Decrypt content of licence file
    std::string strLicense;
    if (!CryptoHelper::decrypt(LicenseManager::m_key, _cryptedLicense, strLicense)) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    License license;
    if (!license.fromString(strLicense))
    {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }    

    // Check keyword
    if (license.getKeyword() != LICENSE_KEYWORD) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    // Check fingerprint
    MachineIdType idType = license.getIdType();
    std::string hashedFingerprint;
    if (!getHashedFingerprint(idType, hashedFingerprint)) {
        return LicenseError::FINGERPRINT_ERROR;
    }
    if (hashedFingerprint != license.getHashedFingerprint()) {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    // Check dates
    int today = dateToInt(DateTime());
    int endValidity = license.getValidityDate();  // >= today
    int lastUsed = license.getLastUsedDate();     // <= today
    if (endValidity < today || lastUsed > today) {
        logger.error("License is invalid");
        return LicenseError::INVALID_LICENSE;
    }

    return LicenseError::VALID_LICENSE;
}


LicenseError LicenseManager::installLicense(const std::string &_request, const std::string &_filename)
{
    // Check license before installation
    LicenseError res = checklicense(_request);
    if(res != LicenseError::VALID_LICENSE) {
        return res;
    }

    // Write result in license file
    std::ofstream file(_filename);
    if (!file.is_open()) {
        LoggerHelper logger;
        logger.error("License file not found.");
        return LicenseError::MISSING_LICENSE_FILE;
    }
    file << _request;
    file.close();

    return LicenseError::INSTALLATION_SUCCESSFUL;
}


LicenseError LicenseManager::generateRequestString(std::string& _request, const std::string& _version)
{
    MachineIdType idType = MachineIdType::UNDEFINED;
    std::string hashedFingerprint;
    if (!getHashedFingerprint(idType, hashedFingerprint))
    {
        return LicenseError::FINGERPRINT_ERROR;
    }

    // Create the request
    LicenseRequest request(idType, hashedFingerprint, DateTime(), _version);

    // Encrypt content of licence file
    std::string fuck = request.toString();
    if(!CryptoHelper::encrypt(LicenseManager::m_key, request.toString(), _request)) {
        LoggerHelper logger;
        logger.error("Cannot encrypt license file.");
        return LicenseError::ERROR_CRYPTO;
    }

    return LicenseError::REQUEST_SUCCESSFUL;
}


LicenseError LicenseManager::rewriteLicense(const std::string &_cryptedLicense, const std::string &_filename)
{
    LoggerHelper logger;

    // Decrypt content of licence file
    std::string strLicense;
    if (!CryptoHelper::decrypt(LicenseManager::m_key, _cryptedLicense, strLicense)) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    License license;
    if (!license.fromString(strLicense))
    {
        logger.error("License is invalid.");
        return LicenseError::INVALID_LICENSE;
    }

    // Update the last used date
    license.update();

    // Encrypt content of licence file
    std::string newCryptedLicense;
    if (!CryptoHelper::encrypt(LicenseManager::m_key, license.toString(), newCryptedLicense)) {
        logger.error("Cannot encrypt licence file.");
        return LicenseError::ERROR_CRYPTO;
    }

    // Write result in license file
    std::ofstream file(_filename);

    if (!file.is_open()) {
        logger.error("Cannot decrypt licence file.");
        return LicenseError::MISSING_LICENSE_FILE;
    }

    file << newCryptedLicense;
    file.close();

    return LicenseError::INSTALLATION_SUCCESSFUL;
}

LicenseRequestError LicenseManager::decryptRequest(const std::string &_encryptedRequest, LicenseRequest& _request)
{
    Tucuxi::Common::LoggerHelper logger;

    // Decrypt content of licence file
    std::string strRequest;
    if (!Tucuxi::Common::CryptoHelper::decrypt(m_key, _encryptedRequest, strRequest)) {
        logger.error("Cannot decrypt request.");
        return LicenseRequestError::ERROR_CRYPTO;
    }

    // Create the request object
    if (!_request.fromString(strRequest)) {
        logger.error("Request is invalid.");
        return LicenseRequestError::INVALID_REQUEST;
    }

    // Some checks...
    if (_request.getKeyword() != LICENSEREQUEST_KEYWORD) {
        logger.error("Request is invalid.");
        return LicenseRequestError::INVALID_REQUEST;
    }

    return LicenseRequestError::REQUEST_SUCCESSFUL;
}


LicenseRequestError LicenseManager::generateLicense(const LicenseRequest &_request, std::string& _license, const Duration& _duration)
{
    Tucuxi::Common::LoggerHelper logger;

    // Build license end date
    DateTime today;
    DateTime endDate = today + _duration;

    // Create the license
    License license(_request, endDate);

    // Generate a crypted license string
    if (!Tucuxi::Common::CryptoHelper::encrypt(m_key, license.toString(), _license) && _license.size() == 0) {
        logger.error("Error encrypt failed.");
        return LicenseRequestError::ERROR_CRYPTO;
    }

    return LicenseRequestError::REQUEST_SUCCESSFUL;
}


bool LicenseManager::getHashedFingerprint(MachineIdType &idType, std::string& _hashedFingerprint)
{
    Tucuxi::Common::LoggerHelper logger;

    // Retrieve fingerprint ID from machine
    std::string fingerprint;
    if (idType == MachineIdType::UNDEFINED) {
        // Look for the first valid id
        for (int i = int(MachineIdType::CPU); i != int(MachineIdType::UNDEFINED); i++) {
            fingerprint = SystemInfo::retrieveFingerPrint(static_cast<MachineIdType>(i));
            if (!fingerprint.empty()) {
                idType = static_cast<MachineIdType>(i);
                break;
            }
        }
    }
    else {
        fingerprint = SystemInfo::retrieveFingerPrint(idType);
    }

    // Check if a valid ID is found
    if (idType == MachineIdType::UNDEFINED) {
        logger.error("No machine id found.");
        return false;
    }

    // Get hash from fingerprint
    if (!CryptoHelper::hash(fingerprint, _hashedFingerprint)) {
        logger.error("Cannot generate hash from fingerprint.");
        return false;
    }

    return true;
}


}
}
