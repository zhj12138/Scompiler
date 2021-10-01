#ifndef SCOMPILER_SRC_BASE_VARIABLE_HPP_
#define SCOMPILER_SRC_BASE_VARIABLE_HPP_

#include "support_type.hpp"

#include <cassert>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class Variable {
  enum class Kind {
    Base,
    Array,
  };
 public:
  Variable(SupportType type, std::string name)
      : kind_(Kind::Base), type_(type), name_(std::move(name)) {}
  Variable(SupportType type, std::vector<int> dimension_vec, std::string name)
      : kind_(Kind::Array), type_(type), name_(std::move(name)), dimension_vec_(std::move(dimension_vec)) {
  }
  ~Variable() = default;
 private:
  Kind kind_;
  SupportType type_;
  std::string name_;
  std::vector<int> dimension_vec_;
};
using VariablePtr = std::shared_ptr<Variable>;

#endif //SCOMPILER_SRC_BASE_VARIABLE_HPP_
