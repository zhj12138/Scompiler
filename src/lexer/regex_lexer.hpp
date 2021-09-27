#ifndef SCOMPILER_SRC_LEXER_REGEX_LEXER_HPP_
#define SCOMPILER_SRC_LEXER_REGEX_LEXER_HPP_

#include "Token.hpp"

#include <string>
#include <vector>

class RegexLexer {
 public:
  std::vector<Token> lex(const std::string &code);
};

#endif //SCOMPILER_SRC_LEXER_REGEX_LEXER_HPP_
