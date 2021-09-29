#ifndef SCOMPILER_SRC_LEXER_TOKEN_HPP_
#define SCOMPILER_SRC_LEXER_TOKEN_HPP_

#include <string>
#include <utility>
#include <variant>
#include <iostream>
#include <cassert>

enum class TokenType {
  /* Keywords */
  Return,
  Int,
  If,
  Else,
  For,
  Do,
  While,
  Break,
  Continue,

  /* Punctuations */
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

  Identifier,
  Number,
  String,
};

// Do not support lineno
class Token {
 public:
  using value_type = std::variant<int, std::string>;

  Token(TokenType type, value_type value) : type_(type), value_(std::move(value)) {}
  ~Token() = default;

  [[nodiscard]] TokenType type() const { return type_; }
  [[nodiscard]] bool is_keyword() const { return type_ >= TokenType::Return && type_ <= TokenType::Continue; }
  [[nodiscard]] bool is_punctuation() const { return type_ >= TokenType::Lparen && type_ <= TokenType::Colon; }
  [[nodiscard]] bool is_identifier() const { return type_ == TokenType::Identifier; }
  [[nodiscard]] bool is_number() const { return type_ == TokenType::Number; }
  [[nodiscard]] bool is_string() const { return type_ == TokenType::String; }

  [[nodiscard]] value_type value() { return value_; }
  [[nodiscard]] int get_number() const { return std::get<int>(value_); }
  [[nodiscard]] std::string get_string() const { return std::get<std::string>(value_); }

  [[nodiscard]] bool is(TokenType t) { return t == type_; }

 private:
  TokenType type_;
  value_type value_;
};


std::ostream &operator<<(std::ostream &os, const Token &token);

Token get_token_by_identifier(const std::string &id);

#endif //SCOMPILER_SRC_LEXER_TOKEN_HPP_
