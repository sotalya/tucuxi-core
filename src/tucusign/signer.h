//@@license@@

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
           std::string country,
           std::string locality,
           std::string orgaName,
           std::string orgaTrustLevel)
        : name(std::move(name)), countryCode(std::move(countryCode)), country(std::move(country)), locality(std::move(locality)),
          organizationName(std::move(orgaName)), organizationTrustLevel(std::move(orgaTrustLevel)){};

    void setName(std::string _name)
    {
        this->name = std::move(_name);
    }

    void setCountryCode(std::string _countryCode)
    {
        this->countryCode = std::move(_countryCode);
    }

    void setCountry(std::string _country)
    {
        this->country = std::move(_country);
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
           << "Country code: " << signer.countryCode << "\n"
           << "Country : " << signer.country << "\n"
           << "Locality: " << signer.locality << "\n"
           << "Organization name: " << signer.organizationName << "\n"
           << "Organization trust level: " << signer.organizationTrustLevel << "\n";
        return os;
    }

private:
    std::string name;
    std::string countryCode;
    std::string country;
    std::string locality;
    std::string organizationName;
    std::string organizationTrustLevel;
};

} // namespace Common
} // namespace Tucuxi

#endif //TUCUXI_SIGNER_H
