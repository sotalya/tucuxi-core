
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

//        Tucuxi::Common::CryptoHelper cryptoHelper;
//        cryptoHelper.encrypt();
    }
};
