/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <iostream>
#include <fstream>

#include "tucucommon/loggerhelper.h"
#include "tucucommon/cryptohelper.h"

#include "tuculicense/licensegenerator.h"

namespace Tucuxi {
namespace License {

// Symmetric key between Tucuxi and Server
const std::string LicenseGenerator::m_key = "86685E7AA62844102FC7FAD5D6DDF46C9CA7777BF4E0153FDF5F86463EAC0D75";

int LicenseGenerator::decryptRequest(std::string _encryptedRequest, Request* _plainRequest)
{
    Tucuxi::Common::LoggerHelper logger;

    std::string request;

    // Decrypt content of licence file
    if(!Tucuxi::Common::CryptoHelper::decrypt(Tucuxi::License::LicenseGenerator::m_key, _encryptedRequest, &request)) {
        logger.error("Cannot decrypt request.");
        return int(Tucuxi::License::RequestError::ERROR_CRYPTO);
    }

    // Read request : key_word:type:fingerprint:request date:version app
    std::size_t field1 = request.find(":");
    std::size_t field2 = request.find(":", field1 + 1);
    std::size_t field3 = request.find(":", field2 + 1);
    std::size_t field4 = request.find(":", field3 + 1);

    // Check the number of fields in license
    if(field1 == std::string::npos ||
            field2 == std::string::npos ||
            field3 == std::string::npos ||
            field4 == std::string::npos) {
        logger.error("Request is invalid.");
        return int(Tucuxi::License::RequestError::ERROR_CRYPTO);
    }

    std::string keyWord     = request.substr(0, field1);
    std::string type        = request.substr(field1 + 1, field2 - field1 - 1);
    std::string fingerprint = request.substr(field2 + 1, field3 - field2 - 1);
    std::string date        = request.substr(field3 + 1, field4 - field3 - 1);
    std::string version     = request.substr(field4 + 1, request.length());

    if(keyWord != "request") {
        logger.error("Request is invalid.");
        return int(Tucuxi::License::RequestError::INVALID_LICENSE);
    }

    _plainRequest->m_keyWord = keyWord;

    try {
        _plainRequest->m_type = std::stoi(type);
    }
    catch (...) {
        logger.error("Request is invalid.");
        return int(Tucuxi::License::RequestError::INVALID_LICENSE);
    }

    _plainRequest->m_fingerprint = fingerprint;
    _plainRequest->m_date = Tucuxi::Common::DateTime(date, "Y-m-d");
    _plainRequest->m_version = version;

    return 0;
}

/// \brief Return a license to be sent to the application.
/// @param _request : request from application.
/// @return license, in case of error : an empty string.
std::string LicenseGenerator::generateLicense(Request _request)
{
    Tucuxi::Common::LoggerHelper logger;

    // Build Date
    Tucuxi::Common::DateTime today;

    std::string endLicense = "";

    if(today.month() + 2 > 12) {
        endLicense = std::to_string(today.year()+1);
    } else {
        endLicense = std::to_string(today.year());
    }

    endLicense += std::to_string((today.month()+2) % 12);
    endLicense += std::to_string(today.day());

    std::string license;

    license = "license:";
    license += std::to_string(_request.m_type);
    license += ":";
    license += _request.m_fingerprint;
    license += ":";
    license += endLicense;
    license += ":";
    license += std::to_string(today.year()) + std::to_string(today.month()) + std::to_string(today.day());

    if(!Tucuxi::Common::CryptoHelper::encrypt(m_key, license, &license))  {
        logger.error("Error encrypt failed.");
        return std::string("");
    }

    return license;
}

}
}
