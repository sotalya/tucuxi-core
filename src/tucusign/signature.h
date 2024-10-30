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


#ifndef TUCUXI_SIGNATURE_H
#define TUCUXI_SIGNATURE_H

#include <string>

namespace Tucuxi {
namespace Sign {

class Signature
{
public:
    Signature(){};

    void setPartial(bool partial)
    {
        this->isPartial = partial;
    }

    void setValue(std::string _value)
    {
        this->value = std::move(_value);
    }

    void setUserCert(std::string _userCertPem)
    {
        this->userCertPem = std::move(_userCertPem);
    }

    void setSigningCert(std::string _signingCertPem)
    {
        this->signingCertPem = std::move(_signingCertPem);
    }

    void setSignedData(std::string _signedData)
    {
        this->signedData = std::move(_signedData);
    }

    std::string getUserCert()
    {
        return this->userCertPem;
    }

    std::string getSigningCert()
    {
        return this->signingCertPem;
    }

    std::string getSignedData()
    {
        return this->signedData;
    }

    std::string getValue()
    {
        return this->value;
    }

    bool getPartial()
    {
        return this->isPartial;
    }

private:
    bool isPartial;
    std::string value;
    std::string userCertPem;
    std::string signingCertPem;
    std::string signedData;
};

} // namespace Sign
} // namespace Tucuxi


#endif //TUCUXI_SIGNATURE_H
