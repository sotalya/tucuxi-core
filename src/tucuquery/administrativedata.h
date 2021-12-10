#ifndef TUCUXI_QUERY_ADMINISTRATIVEDATA_H
#define TUCUXI_QUERY_ADMINISTRATIVEDATA_H

#include <map>
#include <memory>
#include <string>

namespace Tucuxi {
namespace Query {

class Address
{
public:
    // Constructors
    Address() = delete;

    Address(std::string _street, int _postCode, std::string _city, std::string _state, std::string _country);

    Address(Address& _other) = delete;

    // Getters
    const std::string& getStreet() const;
    int getPostCode() const;
    const std::string& getCity() const;
    const std::string& getState() const;
    const std::string& getCountry() const;

protected:
    const std::string m_street;
    const int m_postCode;
    const std::string m_city;
    const std::string m_state;
    const std::string m_country;
};

class Phone
{
public:
    // Constructors
    Phone() = delete;

    Phone(std::string& _number, std::string& _type);

    Phone(Phone& _other) = delete;

    // Getters
    const std::string& getNumber() const;
    const std::string& getType() const;

protected:
    const std::string m_number;
    const std::string m_type;
};

class Email
{
public:
    // Constructors
    Email() = delete;

    Email(std::string& _address, std::string& _type);

    Email(Email& _other) = delete;

    // Getters;
    const std::string& getAddress() const;
    const std::string& getType() const;

protected:
    const std::string m_address;
    const std::string m_type;
};

class PersonalContact
{
public:
    // Constructors
    PersonalContact() = delete;

    PersonalContact(
            std::string& _id,
            std::string& _title,
            std::string& _firstName,
            std::string& _lastName,
            std::unique_ptr<Address> _pAddress,
            std::unique_ptr<Phone> _pPhone,
            std::unique_ptr<Email> _pEmail);

    PersonalContact(PersonalContact& _other) = delete;

    // Getters
    const std::string& getId() const;
    const std::string& getTitle() const;
    const std::string& getFirstName() const;
    const std::string& getLastName() const;
    const Address& getpAddress() const;
    const Phone& getpPhone() const;
    const Email& getpEmail() const;

protected:
    const std::string m_id;
    const std::string m_title;
    const std::string m_firstName;
    const std::string m_lastName;
    std::unique_ptr<Address> m_pAddress;
    std::unique_ptr<Phone> m_pPhone;
    std::unique_ptr<Email> m_pEmail;
};

class InstituteContact
{
public:
    // Constructors
    InstituteContact() = delete;

    InstituteContact(
            std::string& _id,
            std::string& _name,
            std::unique_ptr<Address> _pAddress,
            std::unique_ptr<Phone> _pPhone,
            std::unique_ptr<Email> _pEmail);

    InstituteContact(InstituteContact& _other) = delete;

    // Getters
    const std::string& getId() const;
    const std::string& getName() const;
    const Address& getpAddress() const;
    const Phone& getpPhone() const;
    const Email& getpEmail() const;

protected:
    const std::string m_id;
    const std::string m_name;
    std::unique_ptr<Address> m_pAddress;
    std::unique_ptr<Phone> m_pPhone;
    std::unique_ptr<Email> m_pEmail;
};

class Person
{
public:
    // Constructors
    Person() = delete;

    Person(std::unique_ptr<PersonalContact> _pPerson, std::unique_ptr<InstituteContact> _pInstitute);

    Person(Person& _other) = delete;

    // Getters
    const PersonalContact& getpPerson() const;
    const InstituteContact& getpInstitute() const;

protected:
    std::unique_ptr<PersonalContact> m_pPerson;
    std::unique_ptr<InstituteContact> m_pInstitute;
};

class ClinicalData
{
public:
    // Constructors
    ClinicalData() = delete;

    ClinicalData(std::map<std::string, std::string>& _data);

    ClinicalData(ClinicalData& _other) = delete;

    // Getters
    const std::map<std::string, std::string>& getData() const;

protected:
    const std::map<std::string, std::string> m_data;
};

class AdministrativeData
{
public:
    // Constructors
    AdministrativeData() = delete;

    AdministrativeData(
            std::unique_ptr<Person> _pMandator,
            std::unique_ptr<Person> _pPatient,
            std::unique_ptr<ClinicalData> _pClinicalData);

    AdministrativeData(AdministrativeData& _other) = delete;

    // Getters
    const Person& getpMandator() const;
    const Person& getpPatient() const;
    const ClinicalData& getpClinicalData() const;

protected:
    std::unique_ptr<Person> m_pMandator;
    std::unique_ptr<Person> m_pPatient;
    std::unique_ptr<ClinicalData> m_pClinicalData;
};

} // namespace Query
} // namespace Tucuxi

#endif // TUCUXI_QUERY_ADMINISTRATIVEDATA_H
