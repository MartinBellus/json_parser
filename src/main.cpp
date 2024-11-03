#include <fstream>
#include <iostream>

#include <expr_parser.hpp>
#include <json_parser.hpp>
#include <sstream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<json_file> <expr>" << std::endl;
        return 1;
    }
    std::ifstream json_stream(argv[1]);
    if (!json_stream.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    std::istringstream expr_stream(argv[2]);

    try {
        auto json = json::parse(json_stream);
        auto expr = expr::parse(expr_stream);

        if (expr->ret_type == expr::RetType::INT) {
            std::cout << expr->eval(json.get()) << std::endl;
        } else {
            std::cout << expr->to_string(json.get()) << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
