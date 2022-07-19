//@@license@@

#include "signparser.h"
#include "tucucommon/loggerhelper.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmliterator.h"
#include "tucucommon/xmlnode.h"
#include "signature.h"

namespace Tucuxi {
namespace Common {

ParsingError SignParser::loadSignature(std::string signedDrugfilePath, Signature& signature)
{
    LoggerHelper logger;
    XmlDocument document;
    if (!document.open(signedDrugfilePath)) {
        logger.error("Could not open drug file");
        return ParsingError::UNABLE_TO_LOAD_DRUGFILE;
    }
    else {
        XmlNode root = document.getRoot();
        XmlNodeIterator signatureIterator = root.getChildren("signature");

        if (!signatureIterator->isValid()) {
            logger.error("The drug file has not been signed");
            return ParsingError::NO_SIGNATURE;
        }

        // extract data from xml
        extractSignature(signatureIterator, signature);
        extractSignedData(document, root, signatureIterator, signature);

        return ParsingError::SIGNATURE_OK;
    }
}

void SignParser::extractSignedData(
        Tucuxi::Common::XmlDocument& document,
        Tucuxi::Common::XmlNode& root,
        Tucuxi::Common::XmlNodeIterator signatureIterator,
        Signature& signature)
{
    root.removeNode(*signatureIterator.operator->());

    if (signature.getPartial()) {
        // remove optional nodes
        for (const auto& nodeName : OPTIONAL_NODES) {
            root.removeNodes(nodeName);
            root = document.getRoot();
        }
    }
    // create signed data string
    std::string signedData;
    document.toStringWithDoubleQuote(signedData);
    signature.setSignedData(signedData);
}

void SignParser::extractSignature(Tucuxi::Common::XmlNodeIterator node, Signature& signature)
{
    // get partial attribute
    signature.setPartial(node->getAttribute("partial").getValue() == "true");
    XmlNodeIterator it = node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "signatureValue") {
            signature.setValue(it->getValue());
        }
        else if (nodeName == "certificates") {
            extractCertificates(it, signature);
        }
        it++;
    }
}

void SignParser::extractCertificates(Tucuxi::Common::XmlNodeIterator node, Signature& signature)
{
    XmlNodeIterator it = node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "certificateUser") {
            signature.setUserCert(it->getValue());
        }
        else if (nodeName == "certificateSigning") {
            signature.setSigningCert(it->getValue());
        }
        it++;
    }
}

} // namespace Common
} // namespace Tucuxi
