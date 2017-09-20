/*
* Copyright (C) 2017 Tucuxi SA
*/

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

        bool res;

        std::string key;
        res = Tucuxi::Common::CryptoHelper::generateKey(key);
        std::cout << "Key: " << key << std::endl;

        fructose_assert(res == true);

        std::string credential = "82578DC Gigabit Network Connection00:27:0e:22:16:8fIntel Corporation";
        std::cout << "ID : " << credential << std::endl;

        std::string hashId;
        res = Tucuxi::Common::CryptoHelper::hash(credential, hashId);
        std::cout << "SHA-1: " << hashId << std::endl;

        fructose_assert(res == true);

        std::string test = "Hey !! How are you ? I am an example =)";

        std::string secret;
        res = Tucuxi::Common::CryptoHelper::encrypt(key, test, secret);
        std::cout << "Secret: " << secret << std::endl;

        fructose_assert(res == true);

        std::string plaintext;
        Tucuxi::Common::CryptoHelper::decrypt(key, secret, plaintext);
        std::cout << "Plaintext: " << plaintext << std::endl;

        fructose_assert(test == plaintext);
    }
};
