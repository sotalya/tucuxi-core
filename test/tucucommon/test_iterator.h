/*
* Copyright (C) 2017 Tucuxi SA
*/

#include <vector>

#include "tucucommon/iterator.h"

#include "fructose/fructose.h"

struct TestIterator : public fructose::test_base<TestIterator>
{
    void basic(const std::string& /*_testName*/)
    {
        std::vector<int> lst;
        std::vector<int>::iterator begin = lst.begin();
        std::vector<int>::iterator end = lst.end();
        Tucuxi::Common::StandaloneSTLIterator<std::vector<int>::iterator, int> it(begin, end);
    }
};
