//@@license@@

#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "tucucrypto/cryptohelper.h"

TEST(Crypto_TestCryptoHelper, Basic)
{
    bool res;

    std::string key;
    res = Tucuxi::Common::CryptoHelper::generateKey(key);
    std::cout << "Key: " << key << std::endl;

    ASSERT_TRUE(res);

    std::string credential = "82578DC Gigabit Network Connection00:27:0e:22:16:8fIntel Corporation";
    std::cout << "ID : " << credential << std::endl;

    std::string hashId;
    res = Tucuxi::Common::CryptoHelper::hash(credential, hashId);
    std::cout << "SHA-1: " << hashId << std::endl;

    ASSERT_TRUE(res);

    std::string test = "Hey !! How are you ? I am an example =)";

    std::string secret;
    res = Tucuxi::Common::CryptoHelper::encrypt(key, test, secret);
    std::cout << "Secret: " << secret << std::endl;

    ASSERT_TRUE(res);

    std::string plaintext;
    Tucuxi::Common::CryptoHelper::decrypt(key, secret, plaintext);
    std::cout << "Plaintext: " << plaintext << std::endl;

    ASSERT_EQ(test, plaintext);
}
