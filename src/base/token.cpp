#include "token.hpp"

Token get_token_by_identifier(const std::string &id) {
  if (id == "return") {
    return Token(TokenType::Return, "return");
  } else if (id == "int") {
    return Token(TokenType::Int, "int");
  } else if (id == "if") {
    return Token(TokenType::If, "if");
  } else if (id == "else") {
    return Token(TokenType::Else, "else");
  } else if (id == "for") {
    return Token(TokenType::For, "for");
  } else if (id == "do") {
    return Token(TokenType::Do, "do");
  } else if (id == "while") {
    return Token(TokenType::While, "while");
  } else if (id == "break") {
    return Token(TokenType::Break, "break");
  } else if (id == "continue") {
    return Token(TokenType::Continue, "continue");
  }
  return Token(TokenType::Identifier, id);
}

