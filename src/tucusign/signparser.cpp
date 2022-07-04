//
// Created by fiona on 6/23/22.
//

#include "signparser.h"
#include "signature.h"
#include <iostream>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmliterator.h"

namespace Tucuxi {
namespace Common {

// todo handle error

Signature SignParser::loadSignature(std::string signedDrugfilePath) {
    XmlDocument document;
    if (!document.open(signedDrugfilePath)) {
        std::cerr << "Error";
        exit(1);
    }
    else {
        Signature signature;
        XmlNode root = document.getRoot();
        XmlNodeIterator signatureIterator = root.getChildren("signature");
								// extract data from xml
        extractSignature(signatureIterator, signature);
        extractSignedData(document, root, signatureIterator, signature);

        return signature;
    }
}

void SignParser::extractSignedData(Tucuxi::Common::XmlDocument& document, Tucuxi::Common::XmlNode& root,
                                   Tucuxi::Common::XmlNodeIterator signatureIterator, Signature& signature) {
    root.removeNode(*signatureIterator.operator->());

    if (signature.getPartial()) {
        // remove optional nodes
        std::vector<std::string> nodesName = {"comments", "description"};
        for(const auto& nodeName : nodesName) {
            root.removeNodes(nodeName);
            root = document.getRoot();
        }
    }
    // create signed data string
    std::string signedData;
    document.toStringWithDoubleQuote(signedData);
    signature.setSignedData(signedData);
}

void SignParser::extractSignature(Tucuxi::Common::XmlNodeIterator node, Signature& signature) {
    // get partial attribute
    signature.setPartial(node->getAttribute("partial").getValue() == "true");
    XmlNodeIterator it = node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "signatureValue") {
            signature.setValue(it->getValue());
        } else if (nodeName == "certificates") {
            extractCertificates(it, signature);
        }
        it++;
    }
}

void SignParser::extractCertificates(Tucuxi::Common::XmlNodeIterator node, Signature& signature) {
    XmlNodeIterator it = node->getChildren();

    while (it != XmlNodeIterator::none()) {
        std::string nodeName = it->getName();
        if (nodeName == "certificateUser") {
            signature.setUserCert(it->getValue());
        } else if (nodeName == "certificateSigning") {
            signature.setSigningCert(it->getValue());
        }
        it++;
    }
}

}
}