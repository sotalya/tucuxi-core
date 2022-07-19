//@@license@@

#include <cstring>
#include <sstream>
#include <fstream>
#include "tucucommon/loggerhelper.h"
#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/data_src.h>
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


SignatureError SignValidator::verifySignature(Botan::Public_Key* publicKey, std::string base64Signature, std::string signedData)
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

    if (valid) {
        return SignatureError::SIGNATURE_OK;
    } else {
        return SignatureError::SIGNATURE_NOT_OK;
    }
}


SignatureError SignValidator::verifyChain(Botan::X509_Certificate& userCert, Botan::X509_Certificate& signingCert)
{
    LoggerHelper logger;
    // load ROOT CA certificate in pem format
    std::ifstream ROOTCAFile("./ca.cert.pem");
    if (!ROOTCAFile.is_open()) {
        logger.error("Could not open ROOT CA cert");
        return SignatureError::UNABLE_TO_LOAD_ROOTCA_CERT;
    }
    std::string rootCertPem = std::string(
            (std::istreambuf_iterator<char>(ROOTCAFile)), std::istreambuf_iterator<char>()
                    );

    Botan::X509_Certificate rootCert = loadCert(rootCertPem);

    // define validation restrictions
    std::set<std::string> trusted_hashes;
    trusted_hashes.insert("SHA-256");
    Botan::Path_Validation_Restrictions validation_restrictions(
            false, 80, false, trusted_hashes
            );

    // create the certificate store containing the trusted certificate (the root CA certificate)
    Botan::Certificate_Store_In_Memory root_ca(rootCert);
    std::vector<Botan::Certificate_Store*> trusted_root;
    trusted_root.push_back(&root_ca);

    // create the certificate chain to verify
    std::vector<Botan::X509_Certificate> chain_certs;
    chain_certs.push_back(userCert);
    chain_certs.push_back(signingCert);

    // verify the certificate chain
    Botan::Path_Validation_Result path_validation_result = Botan::x509_path_validate(
            chain_certs, validation_restrictions, trusted_root
            );
    bool valid = path_validation_result.successful_validation();
    std::cout << "Certificate chain is " << (valid ? "valid\n" : "invalid\n");

    if (valid) {
        return SignatureError::CHAIN_OK;
    } else {
        return SignatureError::CHAIN_NOT_OK;
    }

}

SignatureError SignValidator::validateSignature(Signature& signature)
{
    LoggerHelper logger;
    try {
        Botan::X509_Certificate userCert = loadCert(signature.getUserCert());
        Botan::X509_Certificate signingCert = loadCert(signature.getSigningCert());
        Botan::Public_Key* userPublicKey = loadPublicKey(userCert);

        // verify signature
        SignatureError isSignatureValid = verifySignature(
                userPublicKey, signature.getValue(), signature.getSignedData()
        );

        // verify certificate chain
        SignatureError isChainValid = verifyChain(userCert, signingCert);

        if (isSignatureValid == SignatureError::SIGNATURE_OK && isChainValid == SignatureError::CHAIN_OK) {
            return SignatureError::SIGNATURE_VALID;
        } else if (isChainValid == SignatureError::UNABLE_TO_LOAD_ROOTCA_CERT) {
            return SignatureError::UNABLE_TO_LOAD_ROOTCA_CERT;
        } else {
            return SignatureError::SIGNATURE_INVALID;
        }

    } catch (...) {
        logger.error("The drug file is not properly signed");
        return SignatureError::MALFORMED_SIGNATURE;
    }

}

Signer SignValidator::loadSigner(std::string certPem)
{
    try {
        Signer signer;
        Botan::X509_Certificate cert = loadCert(certPem);

        // get certificate subject info
        Botan::X509_DN subject_dn = cert.subject_dn();
        std::multimap<std::string, std::string> subject_dn_attr = subject_dn.contents();
        for (std::multimap<std::string, std::string>::iterator it = subject_dn_attr.begin();
             it != subject_dn_attr.end(); ++it) {
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
            } else if (std::strcmp(it->first.c_str(), "X520.State") == 0) {
                signer.setCountry(it->second);
            }
        }
        return signer;

    } catch(...) {
        return Signer();
    }

}

} // namespace Common
} // namespace Tucuxi
