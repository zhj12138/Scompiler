#ifndef SCOMPILER_SRC_BASE_ERROR_HPP_
#define SCOMPILER_SRC_BASE_ERROR_HPP_

#include <stdexcept>

class lex_error : public std::logic_error {
 public:
  explicit lex_error(const std::string &str) : std::logic_error(str) {}
};

#endif //SCOMPILER_SRC_BASE_ERROR_HPP_
