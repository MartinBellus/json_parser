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
    std::cerr << "\tTest did not panic: " << json << std::endl;
    return false;
}

inline bool test_ok() {
    std::cerr << "Testing test_ok" << std::endl;
    std::string json_str =
            "{\"name\":\"John\"  , \"age\": 30, \n\"car\":[10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);

        test_assert(j->at("name")->to_string() == "John");
        test_assert(j->at("age")->to_int() == 30);
        test_assert(j->at("car")->to_string() == "[10, 20]");
    } catch (const std::exception &e) {
        std::cerr << "\tUnexpected error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

inline bool test_ok_nested() {
    std::cerr << "Testing test_ok_nested" << std::endl;
    std::string json_str = R"({"a": {"b": {  "c": 10}}})";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);

        test_assert(j->at("a")->to_string() == R"({"b": {"c": 10}})");
        test_assert(j->at("a")->at("b")->at("c")->to_int() == 10);
    } catch (const std::exception &e) {
        std::cerr << "\tUnexpected error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

inline bool test_bad_str_key() {
    std::cerr << "Testing test_bad_str_key" << std::endl;
    std::string json_str = R"({"name":"John", "age":30, "car:[10,20]})";
    return test_panics(json_str);
}

inline bool test_bad_str_val() {
    std::cerr << "Testing test_bad_str_val" << std::endl;
    std::string json_str = R"({"name":"John, "age":30, "car":[10,20]})";
    return test_panics(json_str);
}

inline bool test_bad_val() {
    std::cerr << "Testing test_bad_val" << std::endl;
    std::string json_str = R"({"name":"John", "age":30, "car":[m10,20]})";
    return test_panics(json_str);
}

inline void test_all() {
    std::cerr << "Testing json" << std::endl;
    test_assert(test_ok());
    test_assert(test_ok_nested());
    test_assert(test_bad_str_key());
    test_assert(test_bad_str_val());
    test_assert(test_bad_val());
    std::cerr << "All json tests passed\n" << std::endl;
}
} // namespace json_test
#endif
