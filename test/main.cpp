#include <iostream>
#include <json_parser.hpp>

#include "json_test.hpp"

using namespace std;

int main() {
    json_test_all();
    cerr << "All tests passed" << endl;
}
