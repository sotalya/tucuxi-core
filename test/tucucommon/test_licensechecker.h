
#include <string>
#include <iostream>


#include "fructose/fructose.h"

#include "tucucommon/fingerprint.h"
#include "tucucommon/licensechecker.h"

struct TestLicenseChecker : public fructose::test_base<TestLicenseChecker>
{
    std::string m_path;

    TestLicenseChecker(std::string& _path)
    {
        m_path = _path;
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        std::string id;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::CPU);
        std::cout << "CPU : " << id << std::endl;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::MOTHERBOARD);
        std::cout << "MOTHERBOARD : " << id << std::endl;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::BIOS);
        std::cout << "BIOS : " << id << std::endl;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::PRODUCT);
        std::cout << "PRODUCT : " << id << std::endl;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::NETWORK);
        std::cout << "NETWORK : " << id << std::endl;

        id = Tucuxi::Common::FingerPrint::retrieveFingerPrint(Tucuxi::Common::NAME);
        std::cout << "NAME : " << id << std::endl;

        Tucuxi::Common::MachineId machineId;
        Tucuxi::Common::LicenseChecker::retrieveMachineID(&machineId);
        std::cout << "ID selected : " << machineId.m_type << ":" << machineId.m_fingerprint << std::endl;
     }

    void checkValidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::MachineId machineId;
        Tucuxi::Common::LicenseChecker::retrieveMachineID(&machineId);

        std::cout << "ID selected : " << machineId.m_type << ":" << machineId.m_fingerprint << std::endl;

        int res = Tucuxi::Common::LicenseChecker::evalLicenseFile(m_path + std::string("license"), &machineId);
        std::cout << "Res : " << res << std::endl;
    }

    void checkInvalidLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::MachineId machineId;
        Tucuxi::Common::LicenseChecker::retrieveMachineID(&machineId);

        std::cout << "ID selected : " << machineId.m_type << ":" << machineId.m_fingerprint << std::endl;

        int res = Tucuxi::Common::LicenseChecker::evalLicenseFile(m_path + std::string("license2"), &machineId);
        std::cout << "Res : " << res << std::endl;
    }

    void getNewLicense(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        Tucuxi::Common::MachineId machineId;
        Tucuxi::Common::LicenseChecker::retrieveMachineID(&machineId);

        std::cout << "ID selected : " << machineId.m_type << ":" << machineId.m_fingerprint << std::endl;

        int res = Tucuxi::Common::LicenseChecker::writeLicenseFile(m_path + std::string("license"), &machineId);
        std::cout << "Res : " << res << std::endl;
    }

};
