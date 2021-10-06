#ifndef SCOMPILER_SRC_BASE_IR_HPP_
#define SCOMPILER_SRC_BASE_IR_HPP_

#include "ast.hpp"

#include <string>
#include <memory>
#include <list>
#include <utility>
#include <variant>


class IRAddr {
 public:
  // int代表立即数，string代表名字，IRVar代表变量
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

inline IRAddrPtr new_ir_addr(const IRAddr::value_type &v) {
  return std::make_shared<IRAddr>(v);
}

// str代表字符串类型，imm代表数字类型，var代表变量类型, null代表不使用
// Op       a0      a1      a2      作用
// FUNBEG   str     null    null    函数a0开始
// FUNEND   null    null    null    函数结束
// LABEL    imm     null    null    根据a0生成一个唯一的标签
// RET      var|imm null    null    返回a0
// MOV      var     var|imm null    a0 = a1
// NEG      var     var     null    a0 = -a1
// NOT      var     var     null    a0 = ~a1
// LNOT     var     var     null    a0 = !a1
// MUL      var     var|imm var|imm a0 = a1 * a2
// DIV      var     var|imm var|imm a0 = a1 / a2
// REM      var     var|imm var|imm a0 = a1 % a2
// ADD      var     var|imm var|imm a0 = a1 + a2
// SUB      var     var|imm var|imm a0 = a1 - a2
// LT       var     var|imm var|imm a0 = a1 < a2
// GT       var     var|imm var|imm a0 = a1 > a2
// LE       var     var|imm var|imm a0 = (a1 <= a2)
// GE       var     var|imm var|imm a0 = (a1 >= a2)
// EQ       var     var|imm var|imm a0 = (a1 == a2)
// NE       var     var|imm var|imm a0 = (a1 != a2)
// LAND     var     var|imm var|imm a0 = (a1 && a2)
// LOR      var     var|imm var|imm a0 = (a1 || a2)
// JMP      imm     null    null    跳转到a0对应的标签
// BEQZ     var|imm imm     null    如果a0 == 0,则跳转到a1对应的标签
// PARAM    var     null    null    将a0作为参数进行传递
// CALL     var     str     null    调用函数a1，将返回值存放在a0中
// LA       var     str     null    将全局变量a1的地址加载到a0中
// LOAD     var     var     var|imm 以a1为基地址，a2为偏移地址的对应地址的值加载到a0中
// STORE    var     var     var|imm 将a0的值存放到以a1为基地址，a2为偏移地址的对应地址
// ALLOC    var     imm     null    为局部数组分配a1大小的空间，返回首地址到var中
// GBSS     str     imm     null    为全局变量a0分配a1大小的空间(可能是数组)
// GINI     str     imm     null    为全局变量a0分配内存，并初始化为imm(不为数组)
enum class IROp {
  FUNBEG,
  FUNEND,
  LABEL,
  RET,
  MOV,
  NEG,
  NOT,
  LNOT,
  MUL,
  DIV,
  REM,
  ADD,
  SUB,
  LT,
  GT,
  LE,
  GE,
  EQ,
  NE,
  LAND,
  LOR,
  JMP,
  BEQZ,
  PARAM,
  CALL,
  LA,
  LOAD,
  STORE,
  ALLOC,
  GBSS,
  GINI,
};
class IRCode {
 public:
  IRCode(IROp op, IRAddrPtr a0, IRAddrPtr a1, IRAddrPtr a2)
      : op_(op), a0_(std::move(a0)), a1_(std::move(a1)), a2_(std::move(a2)) {}
  ~IRCode() = default;
  IROp &op() { return op_; }
  IRAddrPtr &a0() { return a0_; }
  IRAddrPtr &a1() { return a1_; }
  IRAddrPtr &a2() { return a2_; }
 private:
  IROp op_;
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
  iterator new_ir(IROp op,
                  const IRAddrPtr &a0 = nullptr,
                  const IRAddrPtr &a1 = nullptr,
                  const IRAddrPtr &a2 = nullptr) {
    return new_ir(ircode_list_.end(), op, a0, a1, a2);
  }
  iterator new_ir(iterator pos,
                  IROp op,
                  const IRAddrPtr &a0 = nullptr,
                  const IRAddrPtr &a1 = nullptr,
                  const IRAddrPtr &a2 = nullptr) {
    return ircode_list_.insert(pos, std::make_shared<IRCode>(op, a0, a1, a2));
  }
  IRCodePtr &last_ir() { return ircode_list_.back(); }

  std::list<IRCodePtr> &ircode_list() { return ircode_list_; }
 private:
  std::list<IRCodePtr> ircode_list_;
};
using IRBuilderPtr = std::shared_ptr<IRBuilder>;

// some convert functions
inline IROp to_ir_op(Unary::Op op) {
  switch (op) {
    case Unary::Op::Sub: return IROp::NEG;
    case Unary::Op::Not: return IROp::NOT;
    case Unary::Op::Lnot: return IROp::LNOT;
  }
  assert(false);
}

inline IROp to_ir_op(Multiplicative::Op op) {
  switch (op) {
    case Multiplicative::Op::Times: return IROp::MUL;
    case Multiplicative::Op::Divide: return IROp::DIV;
    case Multiplicative::Op::Mod: return IROp::REM;
  }
  assert(false);
}

inline IROp to_ir_op(Additive::Op op) {
  switch (op) {
    case Additive::Op::Add: return IROp::ADD;
    case Additive::Op::Sub: return IROp::SUB;
  }
  assert(false);
}

inline IROp to_ir_op(Relational::Op op) {
  switch (op) {
    case Relational::Op::Less: return IROp::LT;
    case Relational::Op::Greater: return IROp::GT;
    case Relational::Op::Le: return IROp::LE;
    case Relational::Op::Ge: return IROp::GE;
  }
  assert(false);
}

inline IROp to_ir_op(Equality::Op op) {
  switch (op) {
    case Equality::Op::Equal: return IROp::EQ;
    case Equality::Op::Nequal: return IROp::NE;
  }
  assert(false);
}

#endif //SCOMPILER_SRC_BASE_IR_HPP_
