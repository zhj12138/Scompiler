#ifndef SCOMPILER_SRC_OPTIMIZER_BASIC_BLOCK_HPP_
#define SCOMPILER_SRC_OPTIMIZER_BASIC_BLOCK_HPP_

#include <utility>
#include <set>
#include <deque>

#include "ir.hpp"

class BasicBlock {
  using WeakBasicBlockPtr = std::weak_ptr<BasicBlock>;
 public:
  explicit BasicBlock(std::list<IRCodePtr> ir_list) : ir_list_(std::move(ir_list)) {}

  void calc_use_def();
  void calc_live_variable();

  std::list<IRCodePtr> ir_list_;
  int block_num_{0};
  std::list<WeakBasicBlockPtr> predecessor_list_;  // 前驱基本块
  std::list<WeakBasicBlockPtr> successor_list_;    // 后继基本块

  std::set<IRVar> use_; // 在定值前被使用的变量集合
  std::set<IRVar> def_; // 在使用前被定值的变量集合
  std::set<IRVar> live_variable_IN_;  // 入口处的活跃变量
  std::set<IRVar> live_variable_OUT_; // 出口处的活跃变量

  std::deque<std::set<IRVar>> live_variable_set_deq_;  // 每条IR语句执行之前的活跃变量
};

using BasicBlockPtr = std::shared_ptr<BasicBlock>;
using WeakBasicBlockPtr = std::weak_ptr<BasicBlock>;

inline std::shared_ptr<BasicBlock> make_basic_block(const std::list<IRCodePtr> &ir_list) {
  return std::make_shared<BasicBlock>(ir_list);
}

inline std::shared_ptr<BasicBlock> make_empty_basic_block() {
  return make_basic_block(std::list<IRCodePtr>());
}

#endif //SCOMPILER_SRC_OPTIMIZER_BASIC_BLOCK_HPP_
