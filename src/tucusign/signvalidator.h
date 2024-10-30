/* 
 * Tucuxi - Tucuxi-core library and command line tool. 
 * This code allows to perform prediction of drug concentration in blood 
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV. 
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 * 
 * This program is free software: you can redistribute it and/or modify 
 * it under the terms of the GNU Affero General Public License as 
 * published by the Free Software Foundation, either version 3 of the 
 * License, or any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU Affero General Public License for more details. 
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#ifndef TUCUXI_SIGNATUREVALIDATOR_H
#define TUCUXI_SIGNATUREVALIDATOR_H

#include <iostream>
#include <string>

#include <botan/auto_rng.h>
#include <botan/base64.h>
#include <botan/data_src.h>
#include <botan/hex.h>
#include <botan/pem.h>
#include <botan/pk_keys.h>
#include <botan/pubkey.h>
#include <botan/x509cert.h>
#include <botan/x509path.h>

#include "signature.h"
#include "signer.h"

namespace Tucuxi {
namespace Sign {

enum class SignatureError
{
    SIGNATURE_OK = 1,
    CHAIN_OK = 1,
    SIGNATURE_VALID = 1,
    SIGNATURE_NOT_OK = -1,
    CHAIN_NOT_OK = -1,
    SIGNATURE_INVALID = -1,
    UNABLE_TO_LOAD_ROOTCA_CERT = -2,
    MALFORMED_SIGNATURE = -3,
};

class SignValidator
{
private:
    /// \brief Load a certificate from a certificate in a PEM encoded string
    /// \param certPem The PEM encoded certificate
    /// \return The certificate object
    static Botan::X509_Certificate loadCert(std::string certPem);

    /// \brief Load the public key of a certificate
    /// \param cert The certificate
    /// \return The certificate public key
    static Botan::Public_Key* loadPublicKey(Botan::X509_Certificate& cert);

    /// \brief Verify a signature
    /// \param publicKey The public key
    /// \param base64Signature The signature
    /// \param signedData The data that has been signed
    /// \return SIGNATURE_OK if the signature is valid
    /// SIGNATURE_NOT_OK if the signature is not valid
    static SignatureError verifySignature(
            Botan::Public_Key* publicKey, std::string base64Signature, std::string signedData);

    /// \brief Verify the certificate chain
    /// \param userCert The user certificate
    /// \param signingCert The intermediate certificate
    /// \return CHAIN_OK if the certificate chain is valid
    /// CHAIN_NOT_OK if the certificate chain is not valid
    /// UNABLE_TO_LOAD_ROOTCA_CERT if ROOT CA certificate couldn't be loaded
    static SignatureError verifyChain(Botan::X509_Certificate& userCert, Botan::X509_Certificate& signingCert);

public:
    /// \brief Validate a signature
    /// \param signature The signature to validate
    /// \return SIGNATURE_VALID if the signature is valid
    /// SIGNATURE_INVALID if the signature is not valid
    /// UNABLE_TO_LOAD_ROOT_CA_CERT if ROOT CA certificate couldn't be loaded
    /// MALFORMED_SIGNATURE if the content of the signature is malformed
    static SignatureError validateSignature(Signature& signature);

    /// \brief Load the certificate owner information
    /// \param certPem The PEM encoded certificate
    /// \return The signer object containing the signer information
    /// or a null signer if the certPem is malformed
    static Signer loadSigner(std::string certPem);
};

} // namespace Sign
} // namespace Tucuxi

#endif //TUCUXI_SIGNATUREVALIDATOR_H
