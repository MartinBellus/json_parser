#include <cassert>
#include <iostream>
#include <json_parser.hpp>
#include <sstream>

using namespace std;

bool json_test_ok() {
    string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[10,20]}";
    istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);

        assert(j->at("name")->to_string() == "John");
        assert(j->at("age")->to_int() == 30);
        assert(j->at("car")->to_string() == "[10, 20]");
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}

bool json_test_bad_str_key() {
    string json_str = "{\"name\":\"John\", \"age\":30, \"car:[10,20]}";
    istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 0;
    }
    return 1;
}

bool json_test_bad_str_val() {
    string json_str = "{\"name\":\"John, \"age\":30, \"car\":[10,20]}";
    istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 0;
    }
    return 1;
}

bool json_test_bad_val() {
    string json_str = "{\"name\":\"John\", \"age\":30, \"car\":[m10,20]}";
    istringstream json(json_str);
    try {
        json::json_t j = json::parse(json);
    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 0;
    }
    return 1;
}

int main() {
    assert(!json_test_ok());
    assert(!json_test_bad_str_key());
    assert(!json_test_bad_str_val());
    assert(!json_test_bad_val());
    cerr << "All tests passed" << endl;
}
