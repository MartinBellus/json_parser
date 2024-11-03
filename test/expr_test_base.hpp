#ifndef EXPR_TEST_BASE_H
#define EXPR_TEST_BASE_H

#include <iostream>
#include <string>

#include "test.hpp"
#include <expr_parser.hpp>
#include <json_parser.hpp>

namespace expr_base {

static inline bool test(const std::string &expr, expr::eval_t expected) {
    std::istringstream is(expr.c_str());
    try {
        expr::eval_t result = expr::parse(is)->eval(json::json_t().get());
        if (result != expected) {
            std::cerr << "\t" << expr << " = " << result << " != " << expected
                      << std::endl;
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << "\tUnexpected error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

static inline bool test_panics(const std::string &expr) {
    std::istringstream is(expr.c_str());
    expr::eval_t result;
    try {
        result = expr::parse(is)->eval(json::json_t().get());
    } catch (const std::exception &e) {
        return true;
    }
    std::cerr << "\tDid not panic " << expr << " = " << result << std::endl;
    return false;
}

static inline bool test_ok() {
    std::cerr << "Testing test_ok" << std::endl;
    std::string expr = "1 + 2 * 3";
    expr::eval_t expected = 7;
    return test(expr, expected);
}

static inline bool test_ok_brackets() {
    std::cerr << "Testing test_ok_brackets" << std::endl;
    std::string expr = "10 + (-2 + 4*3)*(12 - 10)";
    expr::eval_t expected = 30;
    return test(expr, expected);
}

static inline bool test_ok_whitespace() {
    std::cerr << "Testing test_ok_whitespace" << std::endl;
    std::string expr = " 1 + 2 *  \n  3 ";
    expr::eval_t expected = 7;
    return test(expr, expected);
}

static inline bool test_panic_noeof1() {
    std::cerr << "Testing test_panic_noeof1" << std::endl;
    std::string expr = "1 + 2 * 3 +3 ()";
    return test_panics(expr);
}

static inline bool test_panic_noeof2() {
    std::cerr << "Testing test_panic_noeof2" << std::endl;
    std::string expr = "1 + 2 * 3 + max(";
    return test_panics(expr);
}

static inline bool test_panic1() {
    std::cerr << "Testing test_panic1" << std::endl;
    std::string expr = "1 + 2 *";
    return test_panics(expr);
}

static inline bool test_panic2() {
    std::cerr << "Testing test_panic2" << std::endl;
    std::string expr = "1 + 2 * (10 + 10";
    return test_panics(expr);
}

inline void test_all() {
    std::cerr << "Testing expr_base" << std::endl;
    test_assert(test_ok());
    test_assert(test_ok_brackets());
    test_assert(test_ok_whitespace());
    test_assert(test_panic_noeof1());
    test_assert(test_panic_noeof2());
    test_assert(test_panic1());
    test_assert(test_panic2());
    std::cerr << "All expr_base tests passed\n" << std::endl;
}
} // namespace expr_base

#endif
