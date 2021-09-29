#include "Token.hpp"

inline std::string to_string(const TokenType &pt) {
  switch (pt) {
    case TokenType::Return: return "Return";
    case TokenType::Int: return "Int";
    case TokenType::If: return "If";
    case TokenType::Else: return "Else";
    case TokenType::For: return "For";
    case TokenType::Do: return "Do";
    case TokenType::While: return "While";
    case TokenType::Break: return "Break";
    case TokenType::Continue: return "Continue";
    case TokenType::Lparen: return "Lparen";
    case TokenType::Rparen: return "Rparen";
    case TokenType::LSbrace: return "LSbrace";
    case TokenType::RSbrace: return "RSbrace";
    case TokenType::LCbrace: return "LCbrace";
    case TokenType::RCbrace: return "RCbrace";
    case TokenType::Semicolon: return "Semicolon";
    case TokenType::Comma: return "Comma";
    case TokenType::Sub: return "Sub";
    case TokenType::Add: return "Add";
    case TokenType::Times: return "Times";
    case TokenType::Divide: return "Divide";
    case TokenType::Mod: return "Mod";
    case TokenType::Not: return "Not";
    case TokenType::Lnot: return "Lnot";
    case TokenType::Or: return "Or";
    case TokenType::Lor: return "Lor";
    case TokenType::And: return "And";
    case TokenType::Land: return "Land";
    case TokenType::Assign: return "Assign";
    case TokenType::Greater: return "Greater";
    case TokenType::Less: return "Less";
    case TokenType::Equal: return "Equal";
    case TokenType::Nequal: return "Nequal";
    case TokenType::Ge: return "Ge";
    case TokenType::Le: return "Le";
    case TokenType::Question: return "Question";
    case TokenType::Colon: return "Colon";
    case TokenType::Identifier:
    case TokenType::Number:
    case TokenType::String: break;
  }
  return "";
}

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

std::ostream &operator<<(std::ostream &os, const Token &token) {
  if (token.is_keyword()) {
    os << "( " << token.get_string() << " , " << to_string(token.type()) << " )";
  } else if (token.is_punctuation()) {
    os << "( " << token.get_string() << " , " << to_string(token.type()) << " )";
  } else if (token.is_identifier()) {
    os << "( " << token.get_string() << " , Identifier )";
  } else if (token.is_number()) {
    os << "( " << token.get_number() << " , Number )";
  } else if (token.is_string()) {
    os << "( " << token.get_string() << " , String )";
  }
  return os;
}
