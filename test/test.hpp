#ifndef TEST_H
#define TEST_H

#include <stdexcept>

inline void test_assert(bool expr) {
    if (!expr) {
        throw std::runtime_error("Assertion failed!");
    }
}

#endif
