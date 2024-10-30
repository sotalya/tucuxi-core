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
