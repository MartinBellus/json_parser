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

enum class RetType {
    INT,
    STR,
    JSON,
};

class Node {
  public:
    Node(RetType type) : ret_type(type) {}
    virtual std::string to_string(json::ref_t json) const = 0;
    virtual eval_t eval(json::ref_t json) const = 0;
    virtual eval_t eval(json::ref_t json, const std::string &func) const {
        if (func == "size") {
            return size(json);
        }
        return eval(json);
    }
    virtual ~Node() = default;
    const RetType ret_type;

  protected:
    virtual eval_t size(json::ref_t json) const = 0;
};

using ptr_t = std::unique_ptr<Node>;

class IntNode : public Node {
  public:
    IntNode(int value) : Node(RetType::INT), value(value) {}
    std::string to_string(json::ref_t json) const override {
        return std::to_string(value);
    }
    eval_t eval(json::ref_t json) const override { return value; }

  protected:
    eval_t size(json::ref_t json) const override { return 1; }

  private:
    int value;
};

class BinaryNode : public Node {
  public:
    BinaryNode(char op, ptr_t &&left, ptr_t &&right)
        : Node(RetType::INT), op(op), left(std::move(left)),
          right(std::move(right)) {}
    std::string to_string(json::ref_t json) const override {
        return "(" + left->to_string(json) + " " + op + " " +
               right->to_string(json) + ")";
    }
    eval_t eval(json::ref_t json) const override {
        switch (op) {
        case '+':
            return left->eval(json) + right->eval(json);
        case '-':
            return left->eval(json) - right->eval(json);
        case '*':
            return left->eval(json) * right->eval(json);
        case '/':
            return left->eval(json) / right->eval(json);
        }
        throw std::runtime_error("EVAL: Unknown binary operator");
    }

  protected:
    eval_t size(json::ref_t json) const override {
        throw std::runtime_error("EVAL: Binary node has no size");
    }

  private:
    ptr_t left, right;
    char op;
};

class UnaryNode : public Node {
  public:
    UnaryNode(char op, ptr_t &&child)
        : Node(RetType::INT), op(op), child(std::move(child)) {}
    std::string to_string(json::ref_t json) const override {
        return (std::string) "(" + op + child->to_string(json) + ")";
    }
    eval_t eval(json::ref_t json) const override {
        switch (op) {
        case '-':
            return -child->eval(json);
        }
        throw std::runtime_error("EVAL: Unknown unary operator");
    }

  protected:
    eval_t size(json::ref_t json) const override {
        throw std::runtime_error("EVAL: Unary node has no size");
    }

  private:
    char op;
    ptr_t child;
};

using args_t = std::vector<ptr_t>;

class FunctionNode : public Node {
  public:
    FunctionNode(std::string &&func, args_t &&args)
        : Node(RetType::INT), func(std::move(func)), args(std::move(args)) {}
    std::string to_string(json::ref_t json) const override {
        std::string result = func + "(";
        for (auto it = args.begin(); it != args.end(); ++it) {
            if (it != args.begin()) {
                result += ", ";
            }
            result += (*it)->to_string(json);
        }
        result += ")";
        return result;
    }
    eval_t eval(json::ref_t json) const override {
        if (func == "size") {
            eval_t result = 0;
            for (const auto &arg : args) {
                result += arg->eval(json, func);
            }
            return result;
        }
        std::vector<eval_t> values;
        values.reserve(args.size());
        if (func == "min") {
            for (const auto &arg : args) {
                values.push_back(arg->eval(json, func));
            }
            return *std::min_element(values.begin(), values.end());
        }
        if (func == "max") {
            for (const auto &arg : args) {
                values.push_back(arg->eval(json, func));
            }
            return *std::max_element(values.begin(), values.end());
        }
        throw std::runtime_error("EVAL: Unknown intrinsic function");
    }

  protected:
    eval_t size(json::ref_t json) const override { return args.size(); }

  private:
    std::string func;
    args_t args;
};

class JsonNode : public Node {
  public:
    JsonNode(std::vector<ptr_t> &&indices)
        : Node(RetType::JSON), indices(std::move(indices)) {}
    std::string to_string(json::ref_t json) const override {
        return get(json)->to_string();
    }
    eval_t eval(json::ref_t json) const override { return get(json)->to_int(); }
    eval_t eval(json::ref_t json, const std::string &func) const override {
        if (func == "size") {
            return size(json);
        }
        auto current = get(json);
        std::vector<int> vals;
        for (const auto &child : current->all()) {
            vals.push_back(child->to_int());
        }
        if (func == "min") {
            return *std::min_element(vals.begin(), vals.end());
        }
        if (func == "max") {
            return *std::max_element(vals.begin(), vals.end());
        }
        throw std::runtime_error("EVAL: Unknown intrinsic function");
    }

  protected:
    eval_t size(json::ref_t json) const override { return get(json)->size(); }

  private:
    json::ref_t get(json::ref_t json) const {
        auto current = json;
        for (const auto &index : indices) {
            switch (index->ret_type) {
            case RetType::INT:
                current = current->at(index->eval(json));
                break;
            case RetType::STR:
                current = current->at(index->to_string(json));
                break;
            case RetType::JSON:
                current = current->at(index->eval(json));
                break;
            }
        }
        return current;
    }
    std::vector<ptr_t> indices;
};

class StringLiteralNode : public Node {
  public:
    StringLiteralNode(std::string &&value)
        : Node(RetType::STR), value(std::move(value)) {}
    std::string to_string(json::ref_t json) const override { return value; }
    eval_t eval(json::ref_t json) const override {
        throw std::runtime_error("EVAL: Cannot evaluate string literal");
    }

  protected:
    eval_t size(json::ref_t json) const override { return value.size(); }

  private:
    std::string value;
};
} // namespace tree

using expr_t = expr::tree::ptr_t;
using RetType = expr::tree::RetType;

expr_t parse(std::istream &is);
} // namespace expr

#endif
