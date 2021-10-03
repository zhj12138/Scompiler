#ifndef SCOMPILER_SRC_BASE_IR_HPP_
#define SCOMPILER_SRC_BASE_IR_HPP_

#include <string>
#include <memory>
#include <list>
#include <utility>
#include <variant>

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

class IRAddr {
 public:
  using value_type = std::variant<int, std::string, IRVar>;
  explicit IRAddr(value_type value) : value_(std::move(value)) {}

  [[nodiscard]] bool is_var() const { return std::holds_alternative<IRVar>(value_); }
  [[nodiscard]] bool is_imm() const { return std::holds_alternative<int>(value_); }
  [[nodiscard]] bool is_name() const { return std::holds_alternative<std::string>(value_); }
  int &imm() { return std::get<int>(value_); }
  IRVar &var() { return std::get<IRVar>(value_); }
  std::string &name() { return std::get<std::string>(value_); }
 private:
  value_type value_;
};
using IRAddrPtr = std::shared_ptr<IRAddr>;

// str代表字符串类型，imm代表数字类型，var代表变量类型, null代表不使用
// Op       a0      a1      a2
// FUNCBEG  str     null    null
// FUNCEND  null    null    null
// MOV      var     var|imm null
// RET      var|imm null    null
class IRCode {
 public:
  enum class Op {
    FUNCBEG,
    FUNCEND,
    RET,
    MOV,
  };
  IRCode(Op op, IRAddrPtr a0, IRAddrPtr a1, IRAddrPtr a2)
      : op_(op), a0_(std::move(a0)), a1_(std::move(a1)), a2_(std::move(a2)) {}
  ~IRCode() = default;
  Op &op() { return op_; }
  IRAddrPtr &a0() { return a0_; }
  IRAddrPtr &a1() { return a1_; }
  IRAddrPtr &a2() { return a2_; }
 private:
  Op op_;
  IRAddrPtr a0_;
  IRAddrPtr a1_;
  IRAddrPtr a2_;
};
using IRCodePtr = std::shared_ptr<IRCode>;

class IRBuilder {
 public:
  using iterator = std::list<IRCodePtr>::iterator;

  IRBuilder() = default;
  ~IRBuilder() = default;

  // 不带迭代器的版本用于中间代码生成
  // 带迭代器的版本用于IR优化
  // 暂时未提供erase相关接口
  iterator new_func_beg(const std::string &func_name);
  iterator new_func_beg(iterator pos, const std::string &func_name);
  iterator new_func_end();
  iterator new_func_end(iterator pos);
  iterator new_mov(const IRVar &var0, const IRVar &var1);
  iterator new_mov(iterator pos, const IRVar &var0, const IRVar &var1);
  iterator new_mov(const IRVar &var0, int imm);
  iterator new_mov(iterator pos, const IRVar &var0, int imm);
  iterator new_ret(const IRVar &var0);
  iterator new_ret(iterator pos, const IRVar &var0);
  iterator new_ret(int imm);
  iterator new_ret(iterator pos, int imm);

  std::list<IRCodePtr> &ircode_list() { return ircode_list_; }
 private:
  std::list<IRCodePtr> ircode_list_;
};
using IRBuilderPtr = std::shared_ptr<IRBuilder>;

#endif //SCOMPILER_SRC_BASE_IR_HPP_
