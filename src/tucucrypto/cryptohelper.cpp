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

#include <botan/auto_rng.h>
#include <botan/filters.h>

#include "tucucrypto/cryptohelper.h"
#ifdef _WIN32
#include <botan/pipe.h>
#else
#include <botan/fd_unix.h>
#endif

#include <botan/hash.h>
#include <botan/hex.h>


namespace Tucuxi {
namespace Common {

bool CryptoHelper::generateKey(std::string& _key)
{
    try {
        // Create a new random key
        Botan::AutoSeeded_RNG rng;
        Botan::SymmetricKey botanKey = rng.random_vec(32);
        _key = botanKey.to_string();
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error in CryptoHelper::generateKey: " << e.what() << std::endl;
        return false;
    }
}

bool CryptoHelper::hash(const std::string& _plaintext, std::string& _result)
{
    try {
        // Get Hash of ID
        std::unique_ptr<Botan::HashFunction> hash(Botan::HashFunction::create("SHA-1"));
        size_t length = _plaintext.length();

        hash->update(reinterpret_cast<const uint8_t*>(&_plaintext[0]), length);

        _result = Botan::hex_encode(hash->final());
        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error in CryptoHelper::hash: " << e.what() << std::endl;
        return false;
    }
}

bool CryptoHelper::encrypt(const std::string& _key, const std::string& _plaintext, std::string& _result)
{
    try {
        // Create the key
        Botan::OctetString data(_key);
        Botan::SymmetricKey masterKey(data);

        // Create the encryption pipe
        Botan::AutoSeeded_RNG rng;
        Botan::InitializationVector iv(rng, 16);

        Botan::Pipe encryptor(
                Botan::get_cipher("AES-256/CBC", masterKey, iv, Botan::ENCRYPTION), new Botan::Base64_Encoder());

        // Encrypt some data
        encryptor.process_msg(_plaintext);
        std::string ciphertext = encryptor.read_all_as_string(0);

        _result = iv.to_string();
        _result += ciphertext;

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error in CryptoHelper::encrypt: " << e.what() << std::endl;
        return false;
    }
}

bool CryptoHelper::decrypt(const std::string& _key, const std::string& _ciphertextiv, std::string& _result)
{
    try {
        // Create the key
        Botan::OctetString data(_key);
        Botan::SymmetricKey masterKey(data);

        Botan::InitializationVector iv(_ciphertextiv.substr(0, 32));
        std::string ciphertext = _ciphertextiv.size() < 32 ? std::string() : _ciphertextiv.substr(32);

        // Create the decryption pipe
        Botan::Pipe decryptor(
                new Botan::Base64_Decoder(), Botan::get_cipher("AES-256/CBC", masterKey, iv, Botan::DECRYPTION));

        // Decrypt some data
        decryptor.process_msg(ciphertext);
        _result = decryptor.read_all_as_string(0);

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "Error in CryptoHelper::decrypt: " << e.what() << std::endl;
        return false;
    }
}

} // namespace Common
} // namespace Tucuxi
