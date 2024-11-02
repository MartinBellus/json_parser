#ifndef JSON_TEST_H
#define JSON_TEST_H

#include <cassert>
#include <iostream>
#include <string>

#include <json_parser.hpp>

inline bool json_test_ok() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);

        assert(j->at("name")->to_string() == "John");
        assert(j->at("age")->to_int() == 30);
        assert(j->at("car")->to_string() == "[10, 20]");
    } catch (const std::exception &e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

inline bool json_test_bad_str_key() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car:[10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const std::exception &e) {
        return 0;
    }
    return 1;
}

inline bool json_test_bad_str_val() {
    std::string json_str = "{\"name\":\"John, \"age\":30, \"car\":[10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const std::exception &e) {
        return 0;
    }
    return 1;
}

inline bool json_test_bad_val() {
    std::string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[m10,20]}";
    std::istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const std::exception &e) {
        return 0;
    }
    return 1;
}
inline void json_test_all() {
    std::cerr << "Testing json" << std::endl;
    assert(!json_test_ok());
    assert(!json_test_bad_str_key());
    assert(!json_test_bad_str_val());
    assert(!json_test_bad_val());
    std::cerr << "All json tests passed" << std::endl << std::endl;
}
#endif
