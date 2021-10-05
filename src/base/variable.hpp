#ifndef SCOMPILER_SRC_BASE_VARIABLE_HPP_
#define SCOMPILER_SRC_BASE_VARIABLE_HPP_

#include "support_type.hpp"

#include <cassert>
#include <string>
#include <memory>
#include <utility>
#include <vector>

class IRVar {
 public:
  using value_type = std::variant<int, std::string>;
  IRVar() = default;
  explicit IRVar(value_type value) : value_(std::move(value)) {}
  [[nodiscard]] bool is_global() const { return std::holds_alternative<std::string>(value_); }
  int &num() { return std::get<int>(value_); }  // 局部变量用数字表示
  std::string &name() { return std::get<std::string>(value_); } // 全局变量用变量名表示
 private:
  value_type value_;
};

class VariableType {
 public:
  explicit VariableType(SupportType type) : type_(type) {}
  VariableType(SupportType type, std::vector<int> dim_vec) : type_(type), dimension_vec_(std::move(dim_vec)) {}

  [[nodiscard]] bool is_array() const { return !dimension_vec_.empty(); }
  SupportType &type() { return type_; }
  std::vector<int> &dimension_vec() { return dimension_vec_; };

  bool operator==(const VariableType &other) const {
    if (type_ != other.type_) return false;
    if (dimension_vec_.size() != other.dimension_vec_.size()) return false;
    return std::equal(dimension_vec_.begin(), dimension_vec_.end(),
                      other.dimension_vec_.begin(), other.dimension_vec_.end(),
                      [](int a, int b) { return a == b; });
  }
  bool operator!=(const VariableType &other) const { return !(*this == other); }

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
  [[nodiscard]] bool allocated() const { return allocated_; }
  IRVar &ir_var() { return ir_var_; }
  void alloc(const IRVar &ir_var) {
    allocated_ = true;
    ir_var_ = ir_var;
  }
 private:
  VariableType type_;
  std::string name_;
  bool allocated_{false}; // 是否分配了IR变量
  IRVar ir_var_;  // 被分配的IR变量
};
using VariablePtr = std::shared_ptr<Variable>;

#endif //SCOMPILER_SRC_BASE_VARIABLE_HPP_
