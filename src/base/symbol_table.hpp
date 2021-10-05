#ifndef SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_
#define SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_

#include "variable.hpp"
#include "ast.hpp"
#include "ir.hpp"

#include <unordered_map>
#include <unordered_set>
#include <utility>

struct FunctionEntry {
 public:
  FunctionEntry(VariableType ret_type, std::string func_name, std::vector<VariableType> param_vec)
      : ret_type_(std::move(ret_type)), func_name_(std::move(func_name)), param_type_vec_(std::move(param_vec)) {}
  explicit FunctionEntry(const FunctionPtr &function)
      : ret_type_(function->ret_type().type()), func_name_(function->name()), param_type_vec_() {
    for (auto &param : function->parameter_list()->variables()) {
      param_type_vec_.push_back(param->type());
    }
  }
  VariableType &ret_type() { return ret_type_; }
  std::string &func_name() { return func_name_; }
  std::vector<VariableType> &param_type_vec() { return param_type_vec_; }

  bool operator==(const FunctionEntry &other) const {
    if (func_name_ != other.func_name_) return false;
    if (ret_type_ != other.ret_type_) return false;
    return std::equal(param_type_vec_.begin(), param_type_vec_.end(),
                      other.param_type_vec_.begin(), other.param_type_vec_.end(),
                      [](const VariableType &lhs, const VariableType &rhs) {
                        return lhs == rhs;
                      });
  }
  bool operator!=(const FunctionEntry &other) const { return !(*this == other); }
 private:
  VariableType ret_type_;
  std::string func_name_;
  std::vector<VariableType> param_type_vec_;
};
using FunctionEntryPtr = std::shared_ptr<FunctionEntry>;

// 函数表
class FunctionTable {
 public:
  FunctionTable() = default;
  ~FunctionTable() = default;
  void define(const FunctionEntryPtr &function_entry) {
    function_map_.emplace(function_entry->func_name(), function_entry);
    define_set_.insert(function_entry->func_name());
  }
  void declare(const FunctionEntryPtr &function_entry) {
    function_map_.emplace(function_entry->func_name(), function_entry);
  }
  bool is_defined(const std::string &func_name) {
    return define_set_.count(func_name);
  }
  FunctionEntryPtr lookup(const std::string &func_name) {
    if (function_map_.count(func_name)) {
      return function_map_[func_name];
    }
    return nullptr;
  }
 private:
  std::unordered_map<std::string, FunctionEntryPtr> function_map_;
  std::unordered_set<std::string> define_set_;
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
  std::pair<VariablePtr, bool> lookup(const std::string &name) {
    if (variable_map_.count(name)) {
      return {variable_map_[name], father_ == nullptr};
    }
    if (father_) {
      return father_->lookup(name);
    }
    return {nullptr, false};
  }
  bool can_use(const std::string &name) {
    if (variable_map_.count(name)) return false;
    return true;
  }
  VariableTablePtr father() const { return father_; }
 private:
  std::unordered_map<std::string, VariablePtr> variable_map_;
  VariableTablePtr father_;
};
using VariableTablePtr = std::shared_ptr<VariableTable>;

class SimpleAllocator {
 public:
  SimpleAllocator() = default;
  ~SimpleAllocator() = default;

  void enter_function() { var_num_ = 0; } // 进入一个新的函数，就重新计数

  int alloc_var() { return var_num_++; }
  int alloc_label() { return label_num_++; }
 private:
  int var_num_{0};
  int label_num_{0};
};

class LoopTable {
  using LoopTablePtr = std::shared_ptr<LoopTable>;
 public:
  LoopTable(LoopTablePtr father, int begin_label, int continue_label, int break_label)
      : father_(std::move(father)),
        begin_label_(begin_label),
        continue_label_(continue_label),
        break_label_(break_label) {}
  ~LoopTable() = default;

  [[nodiscard]] int begin_label() const { return begin_label_; }
  [[nodiscard]] int continue_label() const { return continue_label_; }
  [[nodiscard]] int break_label() const { return break_label_; }
  [[nodiscard]] LoopTablePtr father() const { return father_; }

 private:
  int begin_label_{-1};
  int continue_label_{-1};
  int break_label_{-1};
  LoopTablePtr father_;
};
using LoopTablePtr = std::shared_ptr<LoopTable>;

class SymbolTable {
 public:
  SymbolTable() : function_table_(std::make_shared<FunctionTable>()),
                  variable_table_(std::make_shared<VariableTable>()) {}
  ~SymbolTable() = default;

  void enter() {
    variable_table_ = std::make_shared<VariableTable>(variable_table_);
    ++scope_depth_;
    if (scope_depth_ == 1) allocator_.enter_function();
  }
  void leave() {
    variable_table_ = variable_table_->father();
    --scope_depth_;
  }
  void enter_loop() {
    enter();
    int begin_label = alloc_label();
    int continue_lable = alloc_label();
    int break_label = alloc_label();
    loop_table_ = std::make_shared<LoopTable>(loop_table_, begin_label, continue_lable, break_label);
  }
  void leave_loop() {
    leave();
    assert(loop_table_ != nullptr); // 其实没必要检查
    loop_table_ = loop_table_->father();
  }
  [[nodiscard]] bool in_loop() const { return loop_table_ != nullptr; }
  int loop_begin_label() { return loop_table_->begin_label(); };  // 不检查loop_table_是否为空
  int loop_continue_label() { return loop_table_->continue_label(); };
  int loop_break_label() { return loop_table_->break_label(); };
  void add_variable(const VariablePtr &variable) {
    variable_table_->add(variable);
  }
  void add_function_definition(const FunctionEntryPtr &function) {
    function_table_->define(function);
  }
  void add_function_declaration(const FunctionEntryPtr &function) {
    function_table_->declare(function);
  }
  std::pair<VariablePtr, bool> lookup_variable(const std::string &name) {
    return variable_table_->lookup(name);
  }
  IRVar lookup_ir_var(const std::string &name) {
    auto[variable, is_global] = lookup_variable(name); // 默认是可以查到的，因为checker已经确保这一点
    if (variable->allocated()) {  // 已经分配了IR变量
      return variable->ir_var();
    }
    IRVar new_ir_var;
    if (is_global) {
      new_ir_var = IRVar(name);
    } else {
      new_ir_var = IRVar(alloc_var());
    }
    variable->alloc(new_ir_var);
    return new_ir_var;
  }
  FunctionEntryPtr lookup_function(const std::string &name) {
    return function_table_->lookup(name);
  }
  bool can_use_var_name(const std::string &name) {
    if (function_table_->lookup(name)) return false;  // 变量名不能和函数名重复
    if (!variable_table_->can_use(name)) return false;
    if (in_function_top_level()) {
      if (!variable_table_->father()->can_use(name)) return false;
    }
    return true;
  }
  bool can_use_function_name(const std::string &name) {
    return variable_table_->can_use(name);
  }
  bool function_is_defined(const std::string &name) {
    return function_table_->is_defined(name);
  }
  [[nodiscard]] bool in_function_top_level() const { return scope_depth_ == 2; }
  int alloc_var() { return allocator_.alloc_var(); };
  int alloc_label() { return allocator_.alloc_label(); }
 private:
  FunctionTablePtr function_table_;
  VariableTablePtr variable_table_;
  LoopTablePtr loop_table_;
  SimpleAllocator allocator_;
  int scope_depth_{0};
};

#endif //SCOMPILER_SRC_BASE_SYMBOL_TABLE_HPP_
