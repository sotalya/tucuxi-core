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


#ifndef TUCUXI_SIGNER_H
#define TUCUXI_SIGNER_H

#include <string>

namespace Tucuxi {
namespace Sign {

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
        : name(std::move(name)), countryCode(std::move(countryCode)), country(std::move(country)),
          locality(std::move(locality)), organizationName(std::move(orgaName)),
          organizationTrustLevel(std::move(orgaTrustLevel)){};

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

    std::string getName()
    {
        return name;
    }

    std::string getOrganizationName()
    {
        return organizationName;
    }

    std::string getLocality()
    {
        return locality;
    }

    std::string getCountryCode()
    {
        return countryCode;
    }

private:
    std::string name;
    std::string countryCode;
    std::string country;
    std::string locality;
    std::string organizationName;
    std::string organizationTrustLevel;
};

} // namespace Sign
} // namespace Tucuxi

#endif //TUCUXI_SIGNER_H
