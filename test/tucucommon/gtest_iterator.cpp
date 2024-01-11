//@@license@@

#include <vector>

#include <gtest/gtest.h>

#include "tucucommon/iterator.h"

TEST (Common_TestIterator, Basic){
    std::vector<int> lst;
    std::vector<int>::iterator begin = lst.begin();
    std::vector<int>::iterator end = lst.end();
    Tucuxi::Common::StandaloneSTLIterator<std::vector<int>::iterator, int> it(begin, end);
}
