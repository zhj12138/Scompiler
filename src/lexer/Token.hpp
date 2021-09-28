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

enum class TokenType {
  Keyword,
  Punctuation,
  Identifier,
  Number,
  String,
};

// Do not support lineno
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

 private:
  TokenType type_;
  value_type value_;
};


std::ostream &operator<<(std::ostream &os, const Token &token);

Token get_token_by_identifier(const std::string &id);

#endif //SCOMPILER_SRC_LEXER_TOKEN_HPP_
