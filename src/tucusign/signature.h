//
// Created by fiona on 6/24/22.
//

#ifndef TUCUXI_SIGNATURE_H
#define TUCUXI_SIGNATURE_H

#include <string>

namespace Tucuxi {
namespace Common {

class Signature
{
public:
    Signature() {};

    void setPartial(bool partial) {
        this->isPartial = partial;
    }

    void setValue(std::string _value) {
        this->value = std::move(_value);
    }

    void setUserCert(std::string _userCertPem) {
        this->userCertPem = std::move(_userCertPem);
    }

    void setSigningCert(std::string _signingCertPem) {
        this->signingCertPem = std::move(_signingCertPem);
    }

    void setSignedData(std::string _signedData) {
        this->signedData = std::move(_signedData);
    }

    std::string getUserCert() {
        return this->userCertPem;
    }

    std::string getSigningCert() {
        return this->signingCertPem;
    }

    std::string getSignedData() {
        return this->signedData;
    }

    std::string getValue() {
        return this->value;
    }

    bool getPartial() {
        return this->isPartial;
    }

private:
    bool isPartial;
    std::string value;
    std::string userCertPem;
    std::string signingCertPem;
    std::string signedData;
};

}
}


#endif //TUCUXI_SIGNATURE_H
