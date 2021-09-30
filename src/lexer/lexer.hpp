#ifndef SCOMPILER_LEXER_LEXER_HPP_
#define SCOMPILER_LEXER_LEXER_HPP_

#include "handwritten_lexer.hpp"
#include "spirit_lexer.hpp"
#include "token_stream.hpp"

using Lexer = SpiritLexer;

inline TokenStream lex(const std::string &code) {
  Lexer lexer;
  return TokenStream(lexer.lex(code));
}

inline TokenStream lex_file(const fs::path& file) {
  Lexer lexer;
  return TokenStream(lexer.lex_file(file));
}

#endif //SCOMPILER_LEXER_LEXER_HPP_
