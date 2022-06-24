//
// Created by fiona on 6/23/22.
//

#ifndef TUCUXI_SIGNPARSER_H
#define TUCUXI_SIGNPARSER_H

#include <string>
#include "signature.h"
#include "tucucommon/xmldocument.h"
#include "tucucommon/xmlimporter.h"
#include "tucucommon/xmlnode.h"

namespace Tucuxi {
namespace Common {

class SignParser
{
public:
    SignParser(const std::string signedDrugpath);

    void loadSignature();

private:
    std::string signedDrugpath;

    void extractSignature(Tucuxi::Common::XmlNodeIterator node, Signature& signature);
    void extractCertificates(Tucuxi::Common::XmlNodeIterator node, Signature& signature);
};

}
}

#endif //TUCUXI_SIGNPARSER_H
