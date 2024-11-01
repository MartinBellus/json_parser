#include <json_parser.hpp>
#include <parser.hpp>
#include <stdexcept>

namespace json {

class json_parser : public parser::Parser {
  private:
    friend json_t parse(std::istream &is);
    json_parser(std::istream *_is) : parser::Parser(_is) {}
    json_t object();
    json_t dict();
    json_t list();
    json_t value();
    std::string string();
    int number();
};

json_t json_parser::object() {
    switch (next()) {
    case '{':
        advance();
        return dict();
    case '[':
        advance();
        return list();
    default:
        return value();
    }
}

json_t json_parser::dict() {
    tree::dict_t dict;
    if (next() != '}') {
        std::string key = string();
        if (next() != ':') {
            throw std::runtime_error("':' expected");
        }
        advance();
        dict[key] = object();
    }
    while (next() == ',') {
        advance();
        std::string key = string();
        if (next() != ':') {
            throw std::runtime_error("':' expected");
        }
        advance();
        dict[key] = object();
    }
    if (next() != '}') {
        throw std::runtime_error("'}' expected");
    }
    advance();
    return std::make_unique<tree::DictNode>(std::move(dict));
}

json_t json_parser::list() {
    tree::list_t list;
    if (next() != ']') {
        list.push_back(object());
    }
    while (next() == ',') {
        advance();
        list.push_back(object());
    }
    if (next() != ']') {
        throw std::runtime_error("']' expected");
    }
    advance();
    return std::make_unique<tree::ListNode>(std::move(list));
}

json_t json_parser::value() {
    if (next() == '"') {
        advance();
        return std::make_unique<tree::StringNode>(string());
    }
    if (std::isdigit(next())) {
        return std::make_unique<tree::IntNode>(number());
    }
    throw std::runtime_error("unexpected character when parsing value");
}

std::string json_parser::string() {
    std::string result;
    while (next() != '"') {
        result.push_back(next());
        advance();
    }
    advance();
    return result;
}

int json_parser::number() {
    int result = 0;
    while (std::isdigit(next())) {
        result = result * 10 + next() - '0';
        advance();
    }
    return result;
}

json_t parse(std::istream &is) {
    json_parser parser(&is);
    json_t result = parser.object();
    if (parser.next() != 0) {
        throw std::runtime_error("EOF expected");
    }
    return result;
}
} // namespace json
