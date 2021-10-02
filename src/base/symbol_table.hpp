#ifndef SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_
#define SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_

#include "variable.hpp"

#include <unordered_map>
#include <utility>

struct FunctionEntry {
 public:
  FunctionEntry(VariableType ret_type, std::string func_name, std::vector<VariableType> param_vec)
      : ret_type_(std::move(ret_type)), func_name_(std::move(func_name)), param_vec_(std::move(param_vec)) {}
  VariableType &ret_type() { return ret_type_; }
  std::string &func_name() { return func_name_; }
  std::vector<VariableType> &param_vec() { return param_vec_; }
 private:
  VariableType ret_type_;
  std::string func_name_;
  std::vector<VariableType> param_vec_;
};
using FunctionEntryPtr = std::shared_ptr<FunctionEntry>;

// 函数表
class FunctionTable {
 public:
  FunctionTable() = default;
  ~FunctionTable() = default;
  void add(const FunctionEntryPtr &function_entry) {
    function_map_.emplace(function_entry->func_name(), function_entry);
  }
  FunctionEntryPtr lookup(const std::string &func_name) {
    if (function_map_.count(func_name)) {
      return function_map_[func_name];
    }
    return nullptr;
  }
 private:
  std::unordered_map<std::string, FunctionEntryPtr> function_map_;
};
using FunctionTablePtr = std::shared_ptr<FunctionTable>;

// 变量表
class VariableTable {
  using VariableTablePtr = std::shared_ptr<VariableTable>;
 public:
  explicit VariableTable(VariableTablePtr father = nullptr) : father_(std::move(father)) {}
  ~VariableTable() = default;
  void add(const VariablePtr &variable) {
    variable_map_.emplace(variable->name(), variable);
  }
  VariablePtr lookup(const std::string &name) {
    if (variable_map_.count(name)) {
      return variable_map_[name];
    }
    if (father_) {
      return father_->lookup(name);
    }
    return nullptr;
  }
  bool name_can_use(const std::string &name) {
    if (variable_map_.count(name)) return false;
    return true;
  }
  VariableTablePtr father() const { return father_; }
 private:
  std::unordered_map<std::string, VariablePtr> variable_map_;
  VariableTablePtr father_;
};
using VariableTablePtr = std::shared_ptr<VariableTable>;

class SymbolTable {
 public:
  SymbolTable() : function_table_(std::make_shared<FunctionTable>()),
                  variable_table_(std::make_shared<VariableTable>()) {}
  ~SymbolTable() = default;

  void enter() {
    variable_table_ = std::make_shared<VariableTable>(variable_table_);
  }
  void leave() { variable_table_ = variable_table_->father(); }
  VariablePtr lookup_variable(const std::string &name) {
    return variable_table_->lookup(name);
  }
  FunctionEntryPtr lookup_function(const std::string &name) {
    return function_table_->lookup(name);
  }
  bool name_can_use(const std::string &name) {
    if (variable_table_->name_can_use(name) && function_table_->lookup(name) == nullptr) return true;
    return false;
  }
 private:
  FunctionTablePtr function_table_;
  VariableTablePtr variable_table_;
};

#endif //SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_
