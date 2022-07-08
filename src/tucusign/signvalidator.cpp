//@@license@@

//
// Created by fiona on 6/23/22.
//

#include <cstring>
#include <sstream>

#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/data_src.h>
#include <botan/hex.h>
#include <botan/pem.h>
#include <botan/pk_keys.h>
#include <botan/pubkey.h>
#include <botan/x509cert.h>
#include <botan/x509path.h>

#include "signvalidator.h"

#include "signature.h"
#include "signer.h"

namespace Tucuxi {
namespace Common {

Botan::X509_Certificate SignValidator::loadCert(std::string certPem)
{
    // decode PEM certificate
    std::string label;
    Botan::secure_vector<uint8_t> decoded_pem = Botan::PEM_Code::decode(certPem, label);
    // load data source
    Botan::DataSource_Memory data_source(decoded_pem);
    // create object certificate
    Botan::X509_Certificate cert(data_source);
    return cert;
}

Botan::Public_Key* SignValidator::loadPublicKey(Botan::X509_Certificate& cert)
{
    Botan::Public_Key* public_key(cert.subject_public_key());
    return public_key;
}

Signer SignValidator::loadSigner(std::string certPem)
{
    Botan::X509_Certificate cert = loadCert(certPem);
    Signer signer;
    //"X520.Country""X520.Locality""X520.Organization""X520.State"
    Botan::X509_DN subject_dn = cert.subject_dn();
    std::multimap<std::string, std::string> subject_dn_attr = subject_dn.contents();
    for (std::multimap<std::string, std::string>::iterator it = subject_dn_attr.begin(); it != subject_dn_attr.end();
         ++it) {
        if (std::strcmp(it->first.c_str(), "X520.CommonName") == 0) {
            signer.setName(it->second);
        }
        else if (std::strcmp(it->first.c_str(), "X520.Country") == 0) {
            signer.setCountryCode(it->second);
        }
        else if (std::strcmp(it->first.c_str(), "X520.Locality") == 0) {
            signer.setLocality(it->second);
        }
        else if (std::strcmp(it->first.c_str(), "X520.Organization") == 0) {
            size_t pos = it->second.find_last_of(' ');
            signer.setOrganizationName(it->second.substr(0, pos));
            signer.setOrganizationTrustLevel(it->second.substr(pos + 1));
        }
    }
    return signer;
}

bool SignValidator::verifySignature(Botan::Public_Key* publicKey, std::string base64Signature, std::string signedData)
{
    // decode base 64 signature
    Botan::secure_vector<uint8_t> signature = Botan::base64_decode(base64Signature);

    // convert signed data
    std::vector<uint8_t> data(signedData.data(), signedData.data() + signedData.length());

    // verify signature
    Botan::PK_Verifier verifier(*publicKey, "EMSA1(SHA-256)", Botan::DER_SEQUENCE);
    bool valid = verifier.verify_message(data, signature);
    std::cout << "Signature is " << (valid ? "valid\n" : "invalid\n");

    free(publicKey);

    return valid;
}

bool SignValidator::verifyChain(Botan::X509_Certificate& userCert, Botan::X509_Certificate& signingCert)
{
    // todo read from file
    Botan::X509_Certificate rootCert = loadCert("-----BEGIN CERTIFICATE-----\n"
                                                "MIICtzCCAl2gAwIBAgIULEIMR/HPAXg/PQkhZEXnJXNd98QwCgYIKoZIzj0EAwIw\n"
                                                "gacxCzAJBgNVBAYTAkNIMRQwEgYDVQQIDAtTd2l0emVybGFuZDEQMA4GA1UEBwwH\n"
                                                "WXZlcmRvbjEPMA0GA1UECgwGVHVjdXhpMSUwIwYDVQQLDBxUdWN1eGkgQ2VydGlm\n"
                                                "aWNhdGUgQXV0aG9yaXR5MRcwFQYDVQQDDA5UdWN1eGkgUm9vdCBDQTEfMB0GCSqG\n"
                                                "SIb3DQEJARYQdHVjdXhpQHR1Y3V4aS5jaDAgFw0yMjA2MjkxNTM3MDBaGA8yMDcy\n"
                                                "MDYxNjE1MzcwMFowgacxCzAJBgNVBAYTAkNIMRQwEgYDVQQIDAtTd2l0emVybGFu\n"
                                                "ZDEQMA4GA1UEBwwHWXZlcmRvbjEPMA0GA1UECgwGVHVjdXhpMSUwIwYDVQQLDBxU\n"
                                                "dWN1eGkgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MRcwFQYDVQQDDA5UdWN1eGkgUm9v\n"
                                                "dCBDQTEfMB0GCSqGSIb3DQEJARYQdHVjdXhpQHR1Y3V4aS5jaDBZMBMGByqGSM49\n"
                                                "AgEGCCqGSM49AwEHA0IABFKLloS8GpWxUMcWnkHe6jjIepaS5ftU7D0XqBm2/k18\n"
                                                "djhnJmzI+SX90WnSl56I9xrvSU4s6rFDdbeMFxRZb8OjYzBhMB0GA1UdDgQWBBQc\n"
                                                "1i5wy2MxvV/QUlqK4tNzvTIGtzAfBgNVHSMEGDAWgBQc1i5wy2MxvV/QUlqK4tNz\n"
                                                "vTIGtzAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAKBggqhkjOPQQD\n"
                                                "AgNIADBFAiEAz2T36en6LUblEKpxuj/4x2ubtqKe4KirO4IQVVNm2V8CIAZd/YHe\n"
                                                "Ypl/DHcf4PWOvgXbaK/9mPd0SyygR5qLLbYf\n"
                                                "-----END CERTIFICATE-----");

    // define validation restrictions
    std::set<std::string> trusted_hashes;
    trusted_hashes.insert("SHA-256");
    Botan::Path_Validation_Restrictions validation_restrictions(false, 80, false, trusted_hashes);

    // create the certificate store containing the trusted certificate (the root CA certificate)
    Botan::Certificate_Store_In_Memory root_ca(rootCert);
    std::vector<Botan::Certificate_Store*> trusted_root;
    trusted_root.push_back(&root_ca);

    // create the certificate chain to verify
    std::vector<Botan::X509_Certificate> chain_certs;
    chain_certs.push_back(userCert);
    chain_certs.push_back(signingCert);

    // verify the certificate chain
    Botan::Path_Validation_Result path_validation_result =
            Botan::x509_path_validate(chain_certs, validation_restrictions, trusted_root);
    bool valid = path_validation_result.successful_validation();
    std::cout << "Certificate chain is " << (valid ? "valid\n" : "invalid\n");

    return valid;
}

bool SignValidator::validateSignature(Signature& signature)
{
    Botan::X509_Certificate userCert = loadCert(signature.getUserCert());
    Botan::X509_Certificate signingCert = loadCert(signature.getSigningCert());
    Botan::Public_Key* userPublicKey = loadPublicKey(userCert);

    // verify signature
    bool isSignatureValid = verifySignature(userPublicKey, signature.getValue(), signature.getSignedData());

    // verify certificate chain
    bool isChainValid = verifyChain(userCert, signingCert);

    return isSignatureValid && isChainValid;
}


} // namespace Common
} // namespace Tucuxi
