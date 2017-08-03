
#include <string>
#include <iostream>

#include "fructose/fructose.h"

#include "tucucommon/cryptohelper.h"

struct TestCryptoHelper : public fructose::test_base<TestCryptoHelper>
{
    std::string m_path;

    TestCryptoHelper(std::string& _path)
    {
        m_path = _path;
    }

    void basic(const std::string& _testName)
    {
        std::cout << _testName << std::endl;

        std::string key;
        Tucuxi::Common::CryptoHelper::generateKey(&key);
        std::cout << "Key: " << key << std::endl;

        std::string credential = "82578DC Gigabit Network Connection00:27:0e:22:16:8fIntel Corporation";
        std::cout << "ID : " << credential << std::endl;

        std::string hashId;
        Tucuxi::Common::CryptoHelper::hash(credential, &hashId);
        std::cout << "SHA-1: " << hashId << std::endl;

        std::string secret;
        Tucuxi::Common::CryptoHelper::encrypt(key, hashId, &secret);
        std::cout << "Secret: " << secret << std::endl;

        std::string plaintext;
        Tucuxi::Common::CryptoHelper::decrypt(key, secret, &plaintext);
        std::cout << "Plaintext: " << plaintext << std::endl;

        fructose_assert(hashId == plaintext);
    }
};
