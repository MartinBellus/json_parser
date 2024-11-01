#ifndef PARSER_HPP
#define PARSER_HPP

#include <istream>

namespace parser {
class Parser {
  public:
    Parser(std::istream *is) : is_(is) { advance(); };
    char next() { return next_; }
    void advance() {
        do {
            if (is_->get(next_).eof()) {
                next_ = '\0';
                return;
            }
        } while (std::isspace(next_));
    }

    virtual ~Parser();

    std::istream *is_;
    char next_;
};
} // namespace parser
#endif
