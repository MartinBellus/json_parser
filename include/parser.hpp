#ifndef PARSER_HPP
#define PARSER_HPP

#include <istream>
#include <stdexcept>
#include <string>

namespace parser {
class Parser {
  public:
    Parser(std::istream *is) : is_(is) { advance(); };
    char next() {
        if (next_ == '\0') {
            throw std::runtime_error("Unexpected EOF");
        }
        return next_;
    }
    bool eof() { return next_ == '\0'; }
    void expect(char c) {
        if (next_ != c) {
            throw std::runtime_error((std::string) "expected character " + c);
        }
        advance();
    }
    void advance() {
        do {
            if (is_->get(next_).eof()) {
                next_ = '\0';
                return;
            }
        } while (std::isspace(next_));
    }

    virtual ~Parser() = default;

  private:
    std::istream *is_;
    char next_;
};
} // namespace parser
#endif
