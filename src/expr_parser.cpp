#include "parser.hpp"
#include <expr_parser.hpp>
#include <stdexcept>

namespace expr {

class expr_parser : public parser::Parser {
  public:
    expr_parser(std::istream *is_) : Parser(is_) {}
    expr_t term();
    expr_t add();
    expr_t mul();
    expr_t number();
    expr_t func(std::string &&ident);
    expr_t json_val(std::string &&ident);

    std::string identifier();
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
        std::string ident = identifier();
        if (!eof() && next() == '(') {
            x = func(std::move(ident));
        } else {
            x = json_val(std::move(ident));
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
    auto ind = std::make_unique<tree::StringLiteralNode>(std::move(ident));
    std::vector<expr_t> indices;
    indices.push_back(std::move(ind));
    while (!eof() && (next() == '.' || next() == '[')) {
        if (next() == '.') {
            advance();
            auto ind = std::make_unique<tree::StringLiteralNode>(identifier());
            indices.push_back(std::move(ind));
        } else {
            advance();
            auto ind = add();
            expect(']');
            indices.push_back(std::move(ind));
        }
    }
    return std::make_unique<tree::JsonNode>(std::move(indices));
}

std::string expr_parser::identifier() {
    std::string s;
    while (!eof() && std::isalpha(next())) {
        s.push_back(next());
        advance();
    }
    return s;
}

expr_t parse(std::istream &is) {
    expr_parser parser(&is);
    expr_t result = parser.add();
    if (!parser.eof()) {
        throw std::runtime_error("EXPR_PARSE: EOF expected");
    }
    return result;
}
} // namespace expr
