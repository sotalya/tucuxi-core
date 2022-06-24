//
// Created by fiona on 6/23/22.
//

#include "signvalidator.h"
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

Botan::X509_Certificate SignValidator::loadCert(std::string pemCert) {
    // decode PEM
    std::string label;
    Botan::secure_vector<uint8_t> decoded_pem = Botan::PEM_Code::decode(pemCert, label);
    // load data source
    Botan::DataSource_Memory data_source(decoded_pem);
    // get certificate
    Botan::X509_Certificate cert(data_source);
    return cert;
}


Botan::Public_Key* SignValidator::loadPublicKey(Botan::X509_Certificate cert) {
    // get public key
    Botan::Public_Key* public_key(cert.subject_public_key());
    Botan::X509_DN subject_dn = cert.subject_dn();
    std::multimap< std::string, std::string > subject_dn_attr = subject_dn.contents();
    std::cout << public_key->algo_name() << "\n";
    return public_key;
}

bool SignValidator::verifySignature(Botan::Public_Key* publicKey, std::string base64Signature, std::string drugfile) {
    // decode base 64 signature
    Botan::secure_vector<uint8_t> signature = Botan::base64_decode(base64Signature);

    // convert drugfile
    std::vector<uint8_t> drugfile_(drugfile.data(), drugfile.data() + drugfile.length());

    // verify signature -> FORMAT DER SEQUENCE
    Botan::PK_Verifier verifier(*publicKey, "EMSA1(SHA-256)", Botan::DER_SEQUENCE);
    bool valid = verifier.verify_message(drugfile_, signature);

    std::cout << std::endl << "signature is " << (valid? "valid\n" : "invalid\n");

    free(publicKey);
    return valid;

}

}
}