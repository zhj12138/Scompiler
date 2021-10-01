#ifndef SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_
#define SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_

#include "token.hpp"

#include <vector>

class TokenStream {
 public:
  explicit TokenStream(std::vector<Token> token_vec) : cur_index_(0), token_vec_(std::move(token_vec)) {}
  ~TokenStream() = default;

  using iterator = std::vector<Token>::iterator;
  iterator begin() { return token_vec_.begin(); }
  iterator end() { return token_vec_.end(); }

  void advance(int dis = 1) { cur_index_ += dis; }

  [[nodiscard]] bool is(TokenType type) const { return cur_token().is(type); }
  [[nodiscard]] bool is_type() const { return cur_token().is_type(); }
  [[nodiscard]] bool is_equality_op() const { return cur_token().is_equality_op(); }
  [[nodiscard]] bool is_relational_op() const { return cur_token().is_relational_op(); }
  [[nodiscard]] bool is_additive_op() const { return cur_token().is_additive_op(); }
  [[nodiscard]] bool is_multiplicative_op() const { return cur_token().is_multiplicative_op(); }
  [[nodiscard]] bool is_unary_op() const { return cur_token().is_unary_op(); }

  void consume(TokenType type) {
    assert(is(type));
    advance();
  }
  std::string consume_identifier() {
    assert(is(TokenType::Identifier));
    auto id = token_vec_[cur_index_].get_string();
    advance();
    return id;
  }
  int consume_number() {
    assert(is(TokenType::Identifier));
    auto number = token_vec_[cur_index_].get_number();
    advance();
    return number;
  }

  bool look_ahead(int dis, TokenType type) {
    assert(cur_index_ + dis < token_vec_.size());
    return token_vec_[cur_index_ + dis].is(type);
  }

  bool eof() { return cur_index_ == token_vec_.size(); }

  class Status {
   public:
    explicit Status(int index) : index_(index) {}
    friend class TokenStream;
   private:
    int index_;
  };

  [[nodiscard]] Status store() const { return Status(cur_index_); }
  void restore(Status stored_status) { cur_index_ = stored_status.index_; }

 private:
  [[nodiscard]] Token cur_token() const {
    assert(cur_index_ < token_vec_.size());
    return token_vec_[cur_index_];
  }
  int cur_index_;
  std::vector<Token> token_vec_;
};

#endif //SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_
