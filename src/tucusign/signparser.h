//@@license@@

#ifndef TUCUXI_SIGNPARSER_H
#define TUCUXI_SIGNPARSER_H

#include <string>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

#include "signature.h"

namespace Tucuxi {
namespace Common {

enum class ParsingError
{
    SIGNATURE_OK = 1,
    UNABLE_TO_LOAD_DRUGFILE = -1,
    NO_SIGNATURE = -2,
};

class SignParser
{
public:
    /// \brief Load a Signature from a drug file
    /// \param signedDrugfilePath The signed drug file path
    /// \return SIGNATURE_OK if the signature has been loaded
    /// UNABLE_TO_LOAD_DRUGFILE if the drug file couldn't be loaded
    /// NO_SIGNATURE if the drug file has not been signed (does not contain <signature>)
    static ParsingError loadSignature(std::string signedDrugfilePath, Signature& signature);

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
