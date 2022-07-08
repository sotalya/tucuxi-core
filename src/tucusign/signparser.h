//@@license@@

//
// Created by fiona on 6/23/22.
//

#ifndef TUCUXI_SIGNPARSER_H
#define TUCUXI_SIGNPARSER_H

#include <string>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

#include "signature.h"

namespace Tucuxi {
namespace Common {

class SignParser
{
public:
    /// \brief Load a Signature from a drug file
    /// \param signedDrugfilePath The signed drug file path
    /// \return The Signature object
    static Signature loadSignature(std::string signedDrugfilePath);

private:
    /// \brief Extract the information from the signature node
    /// \param node The signature node
    /// \param signature The signature object to set the information
    static void extractSignature(Tucuxi::Common::XmlNodeIterator node, Signature& signature);

    /// \brief Extract the information from the certificates node
    /// \param node The signature node
    /// \param signature The signature object to set the information
    static void extractCertificates(Tucuxi::Common::XmlNodeIterator node, Signature& signature);

    /// \brief Extract the data that have been signed from the xml document
    /// \param document The xml document
    /// \param root The root node
    /// \param signatureIterator The signature iterator
    /// \param signature The signature object to set the information
    static void extractSignedData(
            Tucuxi::Common::XmlDocument& document,
            Tucuxi::Common::XmlNode& root,
            Tucuxi::Common::XmlNodeIterator signatureIterator,
            Signature& signature);
};

} // namespace Common
} // namespace Tucuxi

#endif //TUCUXI_SIGNPARSER_H
