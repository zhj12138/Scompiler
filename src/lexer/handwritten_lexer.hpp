#ifndef SCOMPILER_SRC_LEXER_HANDWRITTEN_LEXER_HPP_
#define SCOMPILER_SRC_LEXER_HANDWRITTEN_LEXER_HPP_

#include "Token.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class HandwrittenLexer {
 public:
  HandwrittenLexer() = default;
  ~HandwrittenLexer() = default;

  std::vector<Token> lex(const std::string &code);
  std::vector<Token> lex_file(const fs::path &file);
};

#endif //SCOMPILER_SRC_LEXER_HANDWRITTEN_LEXER_HPP_
