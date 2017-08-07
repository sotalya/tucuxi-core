/*
 * Copyright (C) 2017 Tucuxi SA
 */

#include "tucucommon/cryptohelper.h"

#include <stdlib.h>
#include <string.h>

#include <botan/auto_rng.h>
#include <botan/b64_filt.h>
#include <botan/cipher_filter.h>
#ifdef _WIN32
#include <botan/pipe.h>
#else
#include <botan/fd_unix.h>
#endif

#include <botan/hash.h>
#include <botan/hex.h>


namespace Tucuxi {
namespace Common {

bool CryptoHelper::generateKey(std::string* _key)
{
    try {
        // Create a new random key
        Botan::AutoSeeded_RNG rng;
        Botan::SymmetricKey botanKey = rng.random_vec(32);
        *_key = botanKey.as_string();

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool CryptoHelper::hash(std::string _plaintext, std::string* _result)
{
    // Get Hash of ID
    std::unique_ptr<Botan::HashFunction> hash(Botan::HashFunction::create("SHA-1"));
    size_t length = _plaintext.length();

    hash->update(reinterpret_cast<const uint8_t*>(&_plaintext[0]), length);

    *_result = Botan::hex_encode(hash->final());

    return true;
}

bool CryptoHelper::encrypt(std::string _key, std::string _plaintext, std::string* _result)
{
    try {
        // Create the key
        Botan::OctetString data(_key);
        Botan::SymmetricKey masterKey(data);

        // Create the encryption pipe
        Botan::AutoSeeded_RNG rng;
        Botan::InitializationVector iv(rng, 16);

        Botan::Pipe encryptor(Botan::get_cipher("AES-256/CBC", masterKey, iv, Botan::ENCRYPTION),
                              new Botan::Base64_Encoder());

        // Encrypt some data
        encryptor.process_msg(_plaintext);
        std::string ciphertext = encryptor.read_all_as_string(0);

        *_result = iv.as_string();
        *_result += ciphertext;

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool CryptoHelper::decrypt(std::string _key, std::string _ciphertextiv, std::string* _result)
{
    try {
        // Create the key
        Botan::OctetString data(_key);
        Botan::SymmetricKey masterKey(data);

        Botan::InitializationVector iv(_ciphertextiv.substr(0, 32));
        std::string ciphertext = _ciphertextiv.size() < 32 ? std::string() : _ciphertextiv.substr(32);

        // Create the decryption pipe
        Botan::Pipe decryptor(new Botan::Base64_Decoder(),
                              Botan::get_cipher("AES-256/CBC", masterKey, iv, Botan::DECRYPTION));

        // Decrypt some data
        decryptor.process_msg(ciphertext);
        *_result = decryptor.read_all_as_string(0);

        return true;
    }
    catch (...)
    {
        return false;
    }
}

}
}
