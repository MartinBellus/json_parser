#ifndef JSON_TEST_H
#define JSON_TEST_H

#include <iostream>
#include <string>

#include "test.hpp"
#include <json_parser.hpp>

namespace json_test {
static inline bool test_panics(const std::string &json) {
    std::istringstream json_stream(json);
    try {
        json::json_t j = json::parse(json_stream);
    } catch (const std::exception &e) {
        return true;
    }
    std::cerr << "Did not panic " << json << std::endl;
    return false;
}
inline bool test_ok() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);

        test_assert(j->at("name")->to_string() == "John");
        test_assert(j->at("age")->to_int() == 30);
        test_assert(j->at("car")->to_string() == "[10, 20]");
    } catch (const std::exception &e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

inline bool test_bad_str_key() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car:[10,20]}";
    return test_panics(json_str);
}

inline bool test_bad_str_val() {
    std::string json_str = "{\"name\":\"John, \"age\":30, \"car\":[10,20]}";
    return test_panics(json_str);
}

inline bool test_bad_val() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[m10,20]}";
    return test_panics(json_str);
}

inline void test_all() {
    std::cerr << "Testing json" << std::endl;
    test_assert(test_ok());
    test_assert(test_bad_str_key());
    test_assert(test_bad_str_val());
    test_assert(test_bad_val());
    std::cerr << "All json tests passed\n" << std::endl;
}
} // namespace json_test
#endif
