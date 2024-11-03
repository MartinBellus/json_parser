#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <istream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace json {

namespace tree {

class Node;
using ptr_t = std::unique_ptr<Node>;
using dict_t = std::unordered_map<std::string, ptr_t>;
using list_t = std::vector<ptr_t>;
using ref_t = const Node *;

enum class Type { INT, STRING, DICT, LIST };

class Node {
  public:
    Node(Type type) : type(type) {}
    virtual std::string to_string() const = 0;
    virtual int to_int() const = 0;
    virtual size_t size() const = 0;
    virtual std::vector<ref_t> all() const { return {this}; }
    virtual ref_t at(int index) const = 0;
    virtual ref_t at(const std::string &key) const = 0;
    virtual ~Node() = default;
    const Type type;
};

class IntNode : public Node {
  public:
    IntNode(int value) : Node(Type::INT), value(value) {}
    std::string to_string() const override { return std::to_string(value); }
    int to_int() const override { return value; }
    size_t size() const override { return 1; }
    ref_t at(int index) const override {
        throw std::runtime_error("Int is not subscriptable");
    }
    ref_t at(const std::string &key) const override {
        throw std::runtime_error("Int has no keys"); // TODO
    }

  private:
    int value;
};

class StringNode : public Node {
  public:
    StringNode(std::string &&value)
        : Node(Type::STRING), value(std::move(value)) {}
    std::string to_string() const override { return value; }
    int to_int() const override {
        throw std::runtime_error("String can not be converted to int");
    }
    size_t size() const override { return value.size(); }
    ref_t at(int index) const override {
        throw std::runtime_error("String is not subscriptable");
    }
    ref_t at(const std::string &key) const override {
        throw std::runtime_error("String has no keys"); // TODO
    }

  private:
    std::string value;
};

class DictNode : public Node {
  public:
    DictNode(dict_t &&dict) : Node(Type::DICT), dict(std::move(dict)) {}
    std::string to_string() const override {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto &[key, value] : dict) {
            if (!first) {
                ss << ", ";
            }
            if (value->type == Type::STRING) {
                ss << "\"" << key << "\": \"" << value->to_string() << "\"";
            } else {
                ss << "\"" << key << "\": " << value->to_string();
            }
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    int to_int() const override {
        throw std::runtime_error("Dict can not be converted to int");
    };
    size_t size() const override { return dict.size(); }
    std::vector<ref_t> all() const override {
        std::vector<ref_t> refs;
        refs.reserve(dict.size());
        for (const auto &[key, value] : dict) {
            refs.push_back(value.get());
        }
        return refs;
    }
    ref_t at(int index) const override {
        throw std::runtime_error("Dict is not subscriptable");
    }
    ref_t at(const std::string &key) const override {
        auto it = dict.find(key);
        if (it != dict.end()) {
            return it->second.get();
        }
        throw std::runtime_error((std::string) "Key not found: " + key);
    }

  private:
    dict_t dict;
};

class ListNode : public Node {
  public:
    ListNode(list_t &&list) : Node(Type::LIST), list(std::move(list)) {}
    std::string to_string() const override {
        std::stringstream ss;
        ss << "[";
        for (auto it = list.begin(); it != list.end(); ++it) {
            if (it != list.begin()) {
                ss << ", ";
            }
            ss << (*it)->to_string();
        }
        ss << "]";
        return ss.str();
    }
    int to_int() const override {
        throw std::runtime_error("List can not be converted to int");
    };
    size_t size() const override { return list.size(); }
    std::vector<ref_t> all() const override {
        std::vector<ref_t> refs;
        refs.reserve(list.size());
        for (size_t i = 0; i < list.size(); ++i) {
            refs.push_back(list[i].get());
        }
        return refs;
    }
    ref_t at(int index) const override {
        if (index >= 0 && (size_t)index < list.size()) {
            return list[index].get();
        }
        throw std::runtime_error("List index out of range");
    }
    ref_t at(const std::string &key) const override {
        throw std::runtime_error("List is has no keys"); // TODO
    }

  private:
    list_t list;
};

} // namespace tree

using json_t = tree::ptr_t;
using ref_t = tree::ref_t;

json_t parse(std::istream &is);
} // namespace json

#endif
