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


#include "administrativedata.h"

namespace Tucuxi {
namespace Query {

// Address class

Address::Address(std::string _street, int _postCode, std::string _city, std::string _state, std::string _country)
    : m_street(std::move(_street)), m_postCode(_postCode), m_city(std::move(_city)), m_state(std::move(_state)),
      m_country(std::move(_country))
{
}

const std::string& Address::getStreet() const
{
    return m_street;
}

int Address::getPostCode() const
{
    return m_postCode;
}

const std::string& Address::getCity() const
{
    return m_city;
}

const std::string& Address::getState() const
{
    return m_state;
}

const std::string& Address::getCountry() const
{
    return m_country;
}

// Phone class

Phone::Phone(std::string& _number, std::string& _type) : m_number(_number), m_type(_type) {}

const std::string& Phone::getNumber() const
{
    return m_number;
}

const std::string& Phone::getType() const
{
    return m_type;
}

// Email class

Email::Email(std::string& _address, std::string& _type) : m_address(_address), m_type(_type) {}

const std::string& Email::getAddress() const
{
    return m_address;
}

const std::string& Email::getType() const
{
    return m_type;
}

// PersonnalContact class

PersonalContact::PersonalContact(
        std::string& _id,
        std::string& _title,
        std::string& _firstName,
        std::string& _lastName,
        std::unique_ptr<Address> _pAddress,
        std::unique_ptr<Phone> _pPhone,
        std::unique_ptr<Email> _pEmail)
    : m_id(_id), m_title(_title), m_firstName(_firstName), m_lastName(_lastName), m_pAddress(std::move(_pAddress)),
      m_pPhone(std::move(_pPhone)), m_pEmail(std::move(_pEmail))
{
}

const std::string& PersonalContact::getId() const
{
    return m_id;
}

const std::string& PersonalContact::getTitle() const
{
    return m_title;
}

const std::string& PersonalContact::getFirstName() const
{
    return m_firstName;
}

const std::string& PersonalContact::getLastName() const
{
    return m_lastName;
}

const Address& PersonalContact::getpAddress() const
{
    return *m_pAddress;
}

const Phone& PersonalContact::getpPhone() const
{
    return *m_pPhone;
}

const Email& PersonalContact::getpEmail() const
{
    return *m_pEmail;
}

// InstituteContact class

InstituteContact::InstituteContact(
        std::string& _id,
        std::string& _name,
        std::unique_ptr<Address> _pAddress,
        std::unique_ptr<Phone> _pPhone,
        std::unique_ptr<Email> _pEmail)
    : m_id(_id), m_name(_name), m_pAddress(std::move(_pAddress)), m_pPhone(std::move(_pPhone)),
      m_pEmail(std::move(_pEmail))
{
}

const std::string& InstituteContact::getId() const
{
    return m_id;
}

const std::string& InstituteContact::getName() const
{
    return m_name;
}

const Address& InstituteContact::getpAddress() const
{
    return *m_pAddress;
}

const Phone& InstituteContact::getpPhone() const
{
    return *m_pPhone;
}

const Email& InstituteContact::getpEmail() const
{
    return *m_pEmail;
}

// Person class

Person::Person(std::unique_ptr<PersonalContact> _pPerson, std::unique_ptr<InstituteContact> _pInstitute)
    : m_pPerson(std::move(_pPerson)), m_pInstitute(std::move(_pInstitute))
{
}

const PersonalContact& Person::getpPerson() const
{
    return *m_pPerson;
}

const InstituteContact& Person::getpInstitute() const
{
    return *m_pInstitute;
}

// ClinicalData class

ClinicalData::ClinicalData(std::map<std::string, std::string>& _data) : m_data(_data) {}

const std::map<std::string, std::string>& ClinicalData::getData() const
{
    return m_data;
}

// AdministrativeData class

AdministrativeData::AdministrativeData(
        std::unique_ptr<Person> _pMandator,
        std::unique_ptr<Person> _pPatient,
        std::unique_ptr<ClinicalData> _pClinicalData)
    : m_pMandator(std::move(_pMandator)), m_pPatient(std::move(_pPatient)), m_pClinicalData(std::move(_pClinicalData))
{
}

const Person& AdministrativeData::getpMandator() const
{
    return *m_pMandator;
}

const Person& AdministrativeData::getpPatient() const
{
    return *m_pPatient;
}

const ClinicalData& AdministrativeData::getpClinicalData() const
{
    return *m_pClinicalData;
}

} // namespace Query
} // namespace Tucuxi
