
/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include <string>

#include "licensegenerator.h"

using namespace Tucuxi::License;

int main(int argc, char* argv[])
{
	// Check parameters
	if (argc != 3) {
		return -1;
	}
	std::string encryptedRequest = argv[1];
	std::string validityDate = argv[2];

	// Decrypte the license request
	Request licenseRequest;
	RequestError res = LicenseGenerator::decryptRequest(encryptedRequest, licenseRequest);
	if (res != RequestError::REQUEST_SUCCESSFUL) {
		return -2;
	}

	// Generate the license
	std::string license = LicenseGenerator::generateLicense(licenseRequest);
	if (license.size() == 0) {
		return -3;
	}

	// Print out the generated license
	std::cout << license << std::endl;

	return 0;
}