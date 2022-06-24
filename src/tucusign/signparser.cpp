//
// Created by fiona on 6/23/22.
//

#include "signparser.h"
#include "signature.h"
#include "signvalidator.h"
#include <iostream>
#include <regex>

#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlnode.h"
#include "tucucommon/xmliterator.h"

namespace Tucuxi {
namespace Common {

SignParser::SignParser(std::string signedDrugpath) : signedDrugpath(signedDrugpath) {}

// todo clean up
// todo handle error
void SignParser::loadSignature()
{
    XmlDocument document;
    if (!document.open(this->signedDrugpath)) {
        std::cout << "Error";
    }
    else {
        XmlNode root = document.getRoot();

        XmlNodeIterator signatureIterator = root.getChildren("signature");

        Signature signature;

        //checkNodeIterator(drugModelIterator, "drugModel");
        extractSignature(signatureIterator, signature);

        // todo in function
        // remove signature node -> added a function in tucucommon/xmlnode file
        root.removeChildren(*signatureIterator.operator->());
        std::string drugfile;
        document.toString(drugfile);
        // remove whitespace
        drugfile.erase(remove(drugfile.begin(), drugfile.end(), ' '), drugfile.end());
        signature.setDrugfile(drugfile);

        // verify signature
        SignValidator::verifySignature(SignValidator::loadPublicKey(SignValidator::loadCert(signature.getUserCert())), signature.getValue(), signature.getDrugfile());
    }
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
        } else if (nodeName == "certificates") {
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
        } else if (nodeName == "certificateSigning") {
            signature.setSigningCert(it->getValue());
        }
        it++;
    }
}



}
}