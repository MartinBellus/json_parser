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

class Node {
  public:
    virtual std::string to_string() const = 0;
    virtual int to_int() const = 0;
    virtual size_t size() const = 0;
    virtual const Node *operator[](size_t index) const = 0;
    virtual const Node *operator[](const std::string &key) const = 0;
    virtual ~Node();
};

class IntNode : public Node {
  public:
    IntNode(int value) : value(value) {}
    std::string to_string() const override { return std::to_string(value); }
    int to_int() const override { return value; }
    size_t size() const override {
        throw std::runtime_error("Int has no size");
    }
    const Node *operator[](size_t index) const override {
        throw std::runtime_error("Int is not subsciptable");
    }
    const Node *operator[](const std::string &key) const override {
        throw std::runtime_error("Int has no keys"); // TODO
    }

  private:
    int value;
};

class StringNode : public Node {
  public:
    StringNode(std::string &&value) : value(std::move(value)) {}
    std::string to_string() const override { return value; }
    int to_int() const override {
        throw std::runtime_error("String can not be converted to int");
    }
    size_t size() const override {
        throw std::runtime_error("String has no size");
    }
    const Node *operator[](size_t index) const override {
        throw std::runtime_error("String is not subsciptable");
    }
    const Node *operator[](const std::string &key) const override {
        throw std::runtime_error("String has no keys"); // TODO
    }

  private:
    std::string value;
};

class DictNode : public Node {
  public:
    DictNode(dict_t &&dict) : dict(std::move(dict)) {}
    std::string to_string() const override {
        std::stringstream ss;
        ss << "{";
        bool first = true;
        for (const auto &[key, value] : dict) {
            if (!first) {
                ss << ", ";
            }
            ss << "\"" << key << "\": " << value->to_string();
            first = false;
        }
        ss << "}";
        return ss.str();
    }
    int to_int() const override {
        throw std::runtime_error("Dict can not be converted to int");
    };
    size_t size() const override { return dict.size(); }
    const Node *operator[](size_t index) const override {
        throw std::runtime_error("Dict is not subsciptable");
    }
    const Node *operator[](const std::string &key) const override {
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
    ListNode(list_t &&list) : list(std::move(list)) {}
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
    const Node *operator[](size_t index) const override {
        if (list.size() < index) {
            return list[index].get();
        }
        throw std::runtime_error("List index out of range");
    }
    const Node *operator[](const std::string &key) const override {
        throw std::runtime_error("List is has no keys"); // TODO
    }

  private:
    list_t list;
};

} // namespace tree

using json_t = tree::ptr_t;

json_t parse(std::istream &is);
} // namespace json

#endif
