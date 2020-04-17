
/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <string>

#include "tucucrypto/cryptohelper.h"
#include "tucucrypto/systeminfo.h"
#include "tucucrypto/licensemanager.h"

using namespace Tucuxi::Common;
using namespace std::chrono_literals;

int main(int _argc, char* _argv[])
{
    // Check parameters
    if (_argc != 3) {
        std::cout << "Error missing parameters" << std::endl;
        return -1;
    }
    std::string encryptedRequest = _argv[1];
    std::string nbValidityDays = _argv[2];
    
/*    
    // Create the license request 
    std::string encryptedRequest;
    LicenseError res = LicenseManager::generateRequestString(encryptedRequest, "1.0");
    if (res != LicenseError::REQUEST_SUCCESSFUL) {
        std::cout << "Error while creating the request string: " << (int)res << std::endl;
        return -1;
    }
*/
    int nbDays;
    try {
        nbDays = stoi(nbValidityDays);
    }
    catch (...) {
        std::cout << "Error bad validity parameter" << std::endl;
        return -1;
    }
    
    // Decrypte the license request
    LicenseRequest licenseRequest;
    LicenseRequestError reqResult = LicenseManager::decryptRequest(encryptedRequest, licenseRequest);
    if (reqResult != LicenseRequestError::REQUEST_SUCCESSFUL) {
        std::cout << "Error while decrypting the request: " << static_cast<int>(reqResult) << std::endl;
        return -2;
    }
    
    // Generate the license
//    std::cout << "Generate license" << std::endl;
    std::string license;
    reqResult = LicenseManager::generateLicense(licenseRequest, license, Tucuxi::Common::Duration(24h * nbDays));
    if (reqResult != LicenseRequestError::REQUEST_SUCCESSFUL) {
        std::cout << "Error while generating license: " << static_cast<int>(reqResult) << std::endl;
        return -3;
    }
/*
    // Check the generated license
    std::cout << "Checking license" << std::endl;
    LicenseError licResult = LicenseManager::checklicense(license);
    if (licResult != LicenseError::VALID_LICENSE) {
        std::cout << "Error while checking the generated license" << static_cast<int>(reqResult) << std::endl;
        return -4;
    }
*/
    // Print out the generated license
//    std::cout << "License for " << nbDays << ":" << std::endl;
    std::cout << license << std::endl;
    
    return 0;
}
