#ifndef SCOMPILER_SRC_BASE_TOKEN_HPP_
#define SCOMPILER_SRC_BASE_TOKEN_HPP_

#include <string>
#include <utility>
#include <variant>
#include <iostream>
#include <cassert>

enum class TokenType {
  /* Keywords */
  Int,
  Return,
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

  Not,      // ~
  Lnot,     // !

  Sub,      // -
  Add,      // +
  Times,    // *
  Divide,   // /
  Mod,      // %

  Or,       // |
  Lor,      // ||
  And,      // &
  Land,     // &&

  Assign,   // =

  Equal,    // ==
  Nequal,   // !=

  Less,     // <
  Greater,  // >
  Le,       // <=
  Ge,       // >=

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
  [[nodiscard]] bool is_keyword() const { return type_ >= TokenType::Int && type_ <= TokenType::Continue; }
  [[nodiscard]] bool is_punctuation() const { return type_ >= TokenType::Lparen && type_ <= TokenType::Colon; }
  [[nodiscard]] bool is_identifier() const { return type_ == TokenType::Identifier; }
  [[nodiscard]] bool is_number() const { return type_ == TokenType::Number; }
  [[nodiscard]] bool is_string() const { return type_ == TokenType::String; }

  [[nodiscard]] bool is_type() const { return type_ == TokenType::Int; }

  [[nodiscard]] bool is_equality_op() const { return type_ >= TokenType::Equal && type_ <= TokenType::Nequal; }
  [[nodiscard]] bool is_relational_op() const { return type_ >= TokenType::Less && type_ <= TokenType::Ge; }
  [[nodiscard]] bool is_additive_op() const { return type_ >= TokenType::Sub && type_ <= TokenType::Add; }
  [[nodiscard]] bool is_multiplicative_op() const { return type_ >= TokenType::Times && type_ <= TokenType::Mod; };
  [[nodiscard]] bool is_unary_op() const { return type_ >= TokenType::Not && type_ <= TokenType::Sub; }

  [[nodiscard]] value_type value() { return value_; }
  [[nodiscard]] int get_number() const { return std::get<int>(value_); }
  [[nodiscard]] std::string get_string() const { return std::get<std::string>(value_); }

  [[nodiscard]] bool is(TokenType t) const { return t == type_; }

 private:
  TokenType type_;
  value_type value_;
};

std::ostream &operator<<(std::ostream &os, const Token &token);

Token get_token_by_identifier(const std::string &id);

#endif //SCOMPILER_SRC_BASE_TOKEN_HPP_
