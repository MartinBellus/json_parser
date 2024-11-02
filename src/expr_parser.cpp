#include "parser.hpp"
#include <expr_parser.hpp>
#include <stdexcept>

namespace expr {

class expr_parser : public parser::Parser {
  public:
    expr_parser(std::istream *is_, const json::tree::Node *json_root)
        : Parser(is_), json_root(json_root) {}
    expr_t term();
    expr_t add();
    expr_t mul();
    expr_t number();
    expr_t func(std::string &&ident);
    expr_t json_val(std::string &&ident);

    std::string string();

  private:
    const json::tree::Node *json_root;
};

expr_t expr_parser::term() {
    expr_t x;
    if (next() == '(') {
        advance();
        x = add();
        expect(')');
    } else if (std::isdigit(next())) {
        x = number();
    } else if (next() == '-') {
        advance();
        x = std::make_unique<tree::UnaryNode>('-', term());
    } else {
        // func or json
        std::string ident = string();
        switch (next()) {
        case '(':
            x = func(std::move(ident));
            break;
        case '.':
            x = json_val(std::move(ident));
            break;
        case '[':
            x = json_val(std::move(ident));
            break;
        default:
            throw std::runtime_error("unknown token");
        }
    }

    return x;
}

expr_t expr_parser::add() {
    expr_t x = mul();
    while (!eof() && (next() == '+' || next() == '-')) {
        char op = next();
        advance();
        expr_t y = mul();
        x = std::make_unique<tree::BinaryNode>(op, std::move(x), std::move(y));
    }
    return x;
}

expr_t expr_parser::mul() {
    expr_t x = term();
    while (!eof() && (next() == '*' || next() == '/')) {
        char op = next();
        advance();
        expr_t y = term();
        x = std::make_unique<tree::BinaryNode>(op, std::move(x), std::move(y));
    }
    return x;
}

expr_t expr_parser::number() {
    int n = 0;
    while (!eof() && std::isdigit(next())) {
        n = n * 10 + (next() - '0');
        advance();
    }
    return std::make_unique<tree::IntNode>(n);
}

expr_t expr_parser::func(std::string &&ident) {
    expect('(');
    std::vector<expr_t> args;
    if (next() != ')') {
        args.push_back(add());
        while (next() == ',') {
            advance();
            args.push_back(add());
        }
    }
    expect(')');
    return std::make_unique<tree::FunctionNode>(std::move(ident),
                                                std::move(args));
}

expr_t expr_parser::json_val(std::string &&ident) {
    auto current = json_root->at(ident);
    while (next() == '.' || next() == '[') {
        if (next() == '.') {
            advance();
            ident = string();
            current = current->at(ident);
        } else {
            advance();
            int index = add()->eval();
            expect(']');
            current = current->at(index);
        }
    }
    if (current->type == json::tree::Type::INT) {
        return std::make_unique<tree::IntNode>(current->to_int());
    } else {
        return std::make_unique<tree::StringLiteralNode>(current->to_string());
    }
}

std::string expr_parser::string() {
    std::string s;
    while (std::isalpha(next())) {
        s.push_back(next());
        advance();
    }
    return s;
}

expr_t parse(std::istream &is, const json::json_t &json) {
    expr_parser parser(&is, json.get());
    expr_t result = parser.add();
    if (!parser.eof()) {
        throw std::runtime_error("EOF expected");
    }
    return result;
}
} // namespace expr
