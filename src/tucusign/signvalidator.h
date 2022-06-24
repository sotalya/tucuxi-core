//
// Created by fiona on 6/23/22.
//

#ifndef TUCUXI_SIGNATUREVALIDATOR_H
#define TUCUXI_SIGNATUREVALIDATOR_H

#include <string>
#include <iostream>
#include <botan/pk_keys.h>
#include <botan/data_src.h>
#include <botan/pem.h>
#include <botan/x509cert.h>
#include <botan/auto_rng.h>
#include <botan/pubkey.h>
#include <botan/hex.h>
#include <botan/base64.h>
#include <botan/x509path.h>


namespace Tucuxi {
namespace Common {

class SignValidator
{
public:
    static Botan::X509_Certificate loadCert(std::string pemCert);

    static Botan::Public_Key* loadPublicKey(Botan::X509_Certificate cert);

    static bool verifySignature(Botan::Public_Key* publicKey, std::string base64Signature, std::string drugfile);


};

}
}

#endif //TUCUXI_SIGNATUREVALIDATOR_H
