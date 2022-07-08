//@@license@@

//
// Created by fiona on 6/30/22.
//

#ifndef TUCUXI_SIGNER_H
#define TUCUXI_SIGNER_H

#include <string>

namespace Tucuxi {
namespace Common {

class Signer
{
public:
    Signer(){};
    Signer(std::string name,
           std::string countryCode,
           std::string locality,
           std::string orgaName,
           std::string orgaTrustLevel)
        : name(std::move(name)), countryCode(std::move(countryCode)), locality(std::move(locality)),
          organizationName(std::move(orgaName)), organizationTrustLevel(std::move(orgaTrustLevel)){};

    void setName(std::string _name)
    {
        this->name = std::move(_name);
    }

    void setCountryCode(std::string _countryCode)
    {
        this->countryCode = std::move(_countryCode);
    }

    void setLocality(std::string _locality)
    {
        this->locality = std::move(_locality);
    }

    void setOrganizationName(std::string _organizationName)
    {
        this->organizationName = std::move(_organizationName);
    }

    void setOrganizationTrustLevel(std::string _organizationTrustLevel)
    {
        this->organizationTrustLevel = std::move(_organizationTrustLevel);
    }

    friend std::ostream& operator<<(std::ostream& os, const Signer& signer)
    {
        os << "Name: " << signer.name << "\n"
           << "Country: " << signer.countryCode << "\n"
           << "Locality: " << signer.locality << "\n"
           << "Organization name: " << signer.organizationName << "\n"
           << "Organization trust level: " << signer.organizationTrustLevel << "\n";
        return os;
    }

private:
    std::string name;
    std::string countryCode;
    std::string locality;
    std::string organizationName;
    std::string organizationTrustLevel;
};

} // namespace Common
} // namespace Tucuxi

#endif //TUCUXI_SIGNER_H
