#ifndef EXPR_PARSER_HPP
#define EXPR_PARSER_HPP

#include <algorithm>
#include <istream>
#include <json_parser.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace expr {

using eval_t = double;
namespace tree {

class Node {
  public:
    virtual std::string to_string() const = 0;
    virtual eval_t eval() const = 0;
    virtual ~Node() = default;
};

using ptr_t = std::unique_ptr<Node>;

class IntNode : public Node {
  public:
    IntNode(int value) : value(value) {}
    std::string to_string() const override { return std::to_string(value); }
    eval_t eval() const override { return value; }

  private:
    int value;
};

class BinaryNode : public Node {
  public:
    BinaryNode(char op, ptr_t &&left, ptr_t &&right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    std::string to_string() const override {
        return "(" + left->to_string() + " " + op + " " + right->to_string() +
               ")";
    }
    eval_t eval() const override {
        switch (op) {
        case '+':
            return left->eval() + right->eval();
        case '-':
            return left->eval() - right->eval();
        case '*':
            return left->eval() * right->eval();
        case '/':
            return left->eval() / right->eval();
        }
        throw std::runtime_error("Unknown binary operator");
    }

  private:
    ptr_t left, right;
    char op;
};

class UnaryNode : public Node {
  public:
    UnaryNode(char op, ptr_t &&child) : op(op), child(std::move(child)) {}
    std::string to_string() const override {
        return (std::string) "(" + op + child->to_string() + ")";
    }
    eval_t eval() const override {
        switch (op) {
        case '-':
            return -child->eval();
        }
        throw std::runtime_error("Unknown unary operator");
    }

  private:
    char op;
    ptr_t child;
};

using args_t = std::vector<ptr_t>;

class FunctionNode : public Node {
  public:
    FunctionNode(std::string &&func, args_t &&args)
        : func(std::move(func)), args(std::move(args)) {}
    std::string to_string() const override {
        std::string result = func + "(";
        for (const auto &arg : args) {
            result += arg->to_string() + ", ";
        }
        if (!args.empty()) {
            result.pop_back();
            result.pop_back();
        }
        result += ")";
        return result;
    }
    eval_t eval() const override {
        if (func == "size") {
            // TODO
        }
        if (func == "min") {
            std::vector<eval_t> values(args.size());
            for (const auto &arg : args) {
                values.emplace_back(arg->eval());
            }
            return *std::min_element(values.begin(), values.end());
        }
        if (func == "max") {
            std::vector<eval_t> values(args.size());
            for (const auto &arg : args) {
                values.emplace_back(arg->eval());
            }
            return *std::max_element(values.begin(), values.end());
        }
        throw std::runtime_error("Unknown intrinsic function");
    }

  private:
    std::string func;
    args_t args;
};

class StringLiteralNode : public Node {
  public:
    StringLiteralNode(std::string &&value) : value(std::move(value)) {}
    std::string to_string() const override { return value; }
    eval_t eval() const override {
        throw std::runtime_error("Cannot evaluate string literal");
    }

  private:
    std::string value;
};
} // namespace tree

using expr_t = expr::tree::ptr_t;

expr_t parse(std::istream &is, const json::json_t &json);

eval_t eval(std::istream &is, const json::json_t &json);
} // namespace expr

#endif
