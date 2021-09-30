#ifndef SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_
#define SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_

#include "token.hpp"

#include <vector>

class TokenStream {
 public:
  using iterator = std::vector<Token>::iterator;

  explicit TokenStream(std::vector<Token> &&token_vec) {
    token_vec_ = std::move(token_vec);
  }

  iterator begin() { return token_vec_.begin(); }
  iterator end() { return token_vec_.end(); }
 private:
  std::vector<Token> token_vec_;
};

#endif //SCOMPILER_SRC_BASE_TOKEN_STREAM_HPP_
