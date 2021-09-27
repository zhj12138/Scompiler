#ifndef SCOMPILER_SRC_LEXER_TOKEN_HPP_
#define SCOMPILER_SRC_LEXER_TOKEN_HPP_

#include <string>
#include <utility>
#include <variant>
#include <iostream>
#include <cassert>

/* 关键字 */
enum class KeywordType {
  Return,
  Int,
  If,
  Else,
  For,
  Do,
  While,
  Break,
  Continue,
};

inline std::string to_string(const KeywordType &kt) {
  switch (kt) {
    case KeywordType::Return: return "Return";
    case KeywordType::Int: return "Int";
    case KeywordType::If: return "If";
    case KeywordType::Else: return "Else";
    case KeywordType::For: return "For";
    case KeywordType::Do: return "Do";
    case KeywordType::While: return "While";
    case KeywordType::Break: return "Break";
    case KeywordType::Continue: return "Continue";
  }
  assert(false);
}

inline std::string to_keyword(const KeywordType &kt) {
  switch (kt) {
    case KeywordType::Return: return "return";
    case KeywordType::Int: return "int";
    case KeywordType::If: return "if";
    case KeywordType::Else: return "else";
    case KeywordType::For: return "for";
    case KeywordType::Do: return "do";
    case KeywordType::While: return "while";
    case KeywordType::Break: return "break";
    case KeywordType::Continue: return "continue";
  }
  assert(false);
}


/* 标点符号 */
enum class PunctuactionType {
  Lparen,   // (
  Rparen,   // )
  LSbrace,  // [
  RSbrace,  // ]
  LCbrace,  // {
  RCbrace,  // }
  Semicolon,// ;
  Comma,    // ,

  Sub,      // -
  Add,      // +
  Times,    // *
  Divide,   // /
  Mod,      // %

  Not,      // ~
  Lnot,     // !
  Or,       // |
  Lor,      // ||
  And,      // &
  Land,     // &&

  Assign,   // =

  Greater,  // >
  Less,     // <
  Equal,    // ==
  Nequal,   // !=
  Ge,       // >=
  Le,       // <=

  Question, // ?
  Colon,    // :
};

inline std::string to_string(const PunctuactionType &pt) {
  switch (pt) {
    case PunctuactionType::Lparen: return "Lparen";
    case PunctuactionType::Rparen: return "Rparen";
    case PunctuactionType::LSbrace: return "LSbrace";
    case PunctuactionType::RSbrace: return "RSbrace";
    case PunctuactionType::LCbrace: return "LCbrace";
    case PunctuactionType::RCbrace: return "RCbrace";
    case PunctuactionType::Semicolon: return "Semicolon";
    case PunctuactionType::Comma: return "Comma";
    case PunctuactionType::Sub: return "Sub";
    case PunctuactionType::Add: return "Add";
    case PunctuactionType::Times: return "Times";
    case PunctuactionType::Divide: return "Divide";
    case PunctuactionType::Mod: return "Mod";
    case PunctuactionType::Not: return "Not";
    case PunctuactionType::Lnot: return "Lnot";
    case PunctuactionType::Or: return "Or";
    case PunctuactionType::Lor: return "Lor";
    case PunctuactionType::And: return "And";
    case PunctuactionType::Land: return "Land";
    case PunctuactionType::Assign: return "Assign";
    case PunctuactionType::Greater: return "Greater";
    case PunctuactionType::Less: return "Less";
    case PunctuactionType::Equal: return "Equal";
    case PunctuactionType::Nequal: return "Nequal";
    case PunctuactionType::Ge: return "Ge";
    case PunctuactionType::Le: return "Le";
    case PunctuactionType::Question: return "Question";
    case PunctuactionType::Colon: return "Colon";
  }
  assert(false);
}

inline std::string to_punct(const PunctuactionType &pt) {
  switch (pt) {
    case PunctuactionType::Lparen: return "(";
    case PunctuactionType::Rparen: return ")";
    case PunctuactionType::LSbrace: return "[";
    case PunctuactionType::RSbrace: return "]";
    case PunctuactionType::LCbrace: return "{";
    case PunctuactionType::RCbrace: return "}";
    case PunctuactionType::Semicolon: return ";";
    case PunctuactionType::Comma: return ",";
    case PunctuactionType::Sub: return "-";
    case PunctuactionType::Add: return "+";
    case PunctuactionType::Times: return "*";
    case PunctuactionType::Divide: return "/";
    case PunctuactionType::Mod: return "%";
    case PunctuactionType::Not: return "~";
    case PunctuactionType::Lnot: return "!";
    case PunctuactionType::Or: return "|";
    case PunctuactionType::Lor: return "||";
    case PunctuactionType::And: return "&";
    case PunctuactionType::Land: return "&&";
    case PunctuactionType::Assign: return "=";
    case PunctuactionType::Greater: return ">";
    case PunctuactionType::Less: return "<";
    case PunctuactionType::Equal: return "==";
    case PunctuactionType::Nequal: return "!=";
    case PunctuactionType::Ge: return ">=";
    case PunctuactionType::Le: return "<=";
    case PunctuactionType::Question: return "?";
    case PunctuactionType::Colon: return ":";
  }
  assert(false);
}

enum class TokenType {
  Keyword,
  Punctuation,
  Identifier,
  Number,
  String,
};

class Token {
 public:
  using value_type = std::variant<KeywordType, PunctuactionType, int, std::string>;
  Token(TokenType type, value_type value) : type_(type), value_(std::move(value)) {}
  ~Token() = default;

  [[nodiscard]] TokenType type() const { return type_; }
  [[nodiscard]] bool is_keyword() const { return type_ == TokenType::Keyword; }
  [[nodiscard]] bool is_punctuation() const { return type_ == TokenType::Punctuation; }
  [[nodiscard]] bool is_identifier() const { return type_ == TokenType::Identifier; }
  [[nodiscard]] bool is_number() const { return type_ == TokenType::Number; }
  [[nodiscard]] bool is_string() const { return type_ == TokenType::String; }

  value_type value() { return value_; }

  [[nodiscard]] KeywordType get_keyword() const { return std::get<KeywordType>(value_); }
  [[nodiscard]] PunctuactionType get_punctuation() const { return std::get<PunctuactionType>(value_); }
  [[nodiscard]] std::string get_identifier() const { return std::get<std::string>(value_); }
  [[nodiscard]] int get_number() const { return std::get<int>(value_); }
  [[nodiscard]] std::string get_string() const { return std::get<std::string>(value_); }

  friend std::ostream &operator<<(std::ostream &os, const Token &token) {
    if (token.is_keyword()) {
      auto keyword = token.get_keyword();
      os << "( " << to_keyword(keyword) << " , " << to_string(keyword) << " )";
    } else if (token.is_punctuation()) {
      auto punct = token.get_punctuation();
      os << "( " << to_punct(punct) << " , " << to_string(punct) << " )";
    } else if (token.is_identifier()) {
      os << "( " << token.get_identifier() << " , Identifier )";
    } else if (token.is_number()) {
      os << "( " << token.get_number() << " , Number )";
    } else if (token.is_string()) {
      os << "( " << token.get_string() << " , String )";
    }
    return os;
  }
 private:
  TokenType type_;
  value_type value_;
};

#endif //SCOMPILER_SRC_LEXER_TOKEN_HPP_
