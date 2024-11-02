#ifndef EXPR_TEST_H
#define EXPR_TEST_H

#include "test.hpp"
#include <expr_parser.hpp>
#include <iostream>
#include <json_parser.hpp>
#include <sstream>
#include <string>

namespace expr_test {

static inline bool test_int(const std::string &json, const std::string &expr,
                            expr::eval_t expected) {
    std::istringstream json_stream(json), expr_stream(expr);
    try {
        json::json_t json = json::parse(json_stream);
        expr::expr_t expr = expr::parse(expr_stream, json);
        test_assert(expr->eval() == expected);
    } catch (const std::exception &e) {
        std::cerr << "\tUnexpected error: " << e.what() << "\n";
        return false;
    }
    return true;
}

static inline bool test_str(const std::string &json, const std::string &expr,
                            const std::string &expected) {
    std::istringstream json_stream(json), expr_stream(expr);
    try {
        json::json_t json = json::parse(json_stream);
        expr::expr_t expr = expr::parse(expr_stream, json);
        test_assert(expr->to_string() == expected);
    } catch (const std::exception &e) {
        std::cerr << "\tUnexpected error: " << e.what() << "\n";
        return false;
    }
    return true;
}

static inline bool test_panic(const std::string &json,
                              const std::string &expr) {
    std::istringstream json_stream(json), expr_stream(expr);
    try {
        json::json_t json = json::parse(json_stream);
        expr::expr_t expr = expr::parse(expr_stream, json);
        test_assert(false);
    } catch (const std::exception &e) {
        return true;
    }
    std::cerr << "\tTest did not panic: " << json << " " << expr << std::endl;
    return false;
}

inline std::string example_json =
        R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

static inline bool test_example1() {
    std::cerr << "Testing test_example_1" << std::endl;
    std::string expr = R"(a.b[1])";
    return test_int(example_json, expr, 2);
}

static inline bool test_example2() {
    std::cerr << "Testing test_example_2" << std::endl;
    std::string expr = R"(a.b[2].c)";
    return test_str(example_json, expr, "test");
}

static inline bool test_example3() {
    std::cerr << "Testing test_example_3" << std::endl;
    std::string expr = R"(a.b)";
    return test_str(example_json, expr, R"([1, 2, {"c": "test"}, [11, 12]])");
}

static inline bool test_example_nested() {
    std::cerr << "Testing test_example_nested" << std::endl;
    std::string expr = R"(a.b[a.b[1]].c)";
    return test_str(example_json, expr, "test");
}

static inline bool test_example_func1() {
    std::cerr << "Testing test_example_func1" << std::endl;
    std::string expr = R"(max(a.b[0], a.b[1]))";
    return test_int(example_json, expr, 2);
}

static inline bool test_example_func2() {
    std::cerr << "Testing test_example_func2" << std::endl;
    std::string expr = R"(min(a.b[3]))";
    return test_int(example_json, expr, 11);
}

static inline bool test_example_func3() {
    std::cerr << "Testing test_example_func3" << std::endl;
    std::string expr = R"(size(a))";
    return test_int(example_json, expr, 1);
}

static inline bool test_example_func4() {
    std::cerr << "Testing test_example_func4" << std::endl;
    std::string expr = R"(size(a.b))";
    return test_int(example_json, expr, 4);
}

static inline bool test_example_func5() {
    std::cerr << "Testing test_example_func5" << std::endl;
    std::string expr = R"(size(a.b[a.b[1]].c))";
    return test_int(example_json, expr, 4);
}

static inline bool test_example_func_literals() {
    std::cerr << "Testing test_example_func_literals" << std::endl;
    std::string expr = R"(max(a.b[0], 10, a.b[1], 15))";
    return test_int(example_json, expr, 15);
}

inline void test_all() {
    std::cerr << "Testing expr" << std::endl;
    test_assert(test_example1());
    test_assert(test_example2());
    test_assert(test_example3());
    test_assert(test_example_nested());
    test_assert(test_example_func1());
    test_assert(test_example_func2());
    test_assert(test_example_func3());
    test_assert(test_example_func4());
    test_assert(test_example_func5());
    test_assert(test_example_func_literals());
    std::cerr << "All expr tests passed\n" << std::endl;
}
} // namespace expr_test

#endif