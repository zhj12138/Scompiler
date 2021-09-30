#ifndef SCOMPILER_SRC_LEXER_SPIRIT_LEXER_HPP_
#define SCOMPILER_SRC_LEXER_SPIRIT_LEXER_HPP_

#include "token.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class SpiritLexer {
 public:
  std::vector<Token> lex(const std::string &code);
  std::vector<Token> lex_file(const fs::path &file);
};

#endif //SCOMPILER_SRC_LEXER_SPIRIT_LEXER_HPP_
