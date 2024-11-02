#include <iostream>
#include <json_parser.hpp>

#include "expr_test_base.hpp"
#include "json_test.hpp"

using namespace std;

int main() {
    try {
        expr_base::test_all();
        json_test::test_all();
    } catch (const exception &e) {
        return 1;
    }
    cerr << "All tests passed" << endl;
}
