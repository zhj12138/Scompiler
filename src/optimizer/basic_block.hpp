#ifndef SCOMPILER_SRC_OPTIMIZER_BASIC_BLOCK_HPP_
#define SCOMPILER_SRC_OPTIMIZER_BASIC_BLOCK_HPP_

#include <utility>

#include "ir.hpp"

class BasicBlock {
  using WeakBasicBlockPtr = std::weak_ptr<BasicBlock>;
 public:
  explicit BasicBlock(std::list<IRCodePtr> ir_list) : ir_list_(std::move(ir_list)) {}
  std::list<IRCodePtr> ir_list_;
  std::list<WeakBasicBlockPtr> predecessor_list_;  // 前驱基本块
  std::list<WeakBasicBlockPtr> successor_list_;    // 后继基本块
  int block_num_{0};
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
