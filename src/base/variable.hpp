#ifndef SCOMPILER_SRC_BASE_VARIABLE_HPP_
#define SCOMPILER_SRC_BASE_VARIABLE_HPP_

#include "support_type.hpp"

#include <cassert>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class VariableType {
 public:
  explicit VariableType(SupportType type) : type_(type) {}
  VariableType(SupportType type, std::vector<int> dim_vec) : type_(type), dimension_vec_(std::move(dim_vec)) {}

  [[nodiscard]] bool is_array() const { return !dimension_vec_.empty(); }
  SupportType &type() { return type_; }
  std::vector<int> &dimension_vec() { return dimension_vec_; };
 private:
  SupportType type_;
  std::vector<int> dimension_vec_;
};

class Variable {
 public:
  Variable(SupportType type, std::string name)
      : type_(type), name_(std::move(name)) {}
  Variable(SupportType type, std::vector<int> dimension_vec, std::string name)
      : type_(type, std::move(dimension_vec)), name_(std::move(name)) {
  }
  ~Variable() = default;

  VariableType &type() { return type_; }
  std::string &name() { return name_; }
 private:
  VariableType type_;
  std::string name_;
};
using VariablePtr = std::shared_ptr<Variable>;

#endif //SCOMPILER_SRC_BASE_VARIABLE_HPP_
