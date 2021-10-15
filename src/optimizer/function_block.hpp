#ifndef SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_
#define SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_

#include "basic_block.hpp"
#include <vector>

class FunctionBlock {
 public:
  explicit FunctionBlock(std::list<IRCodePtr> ir_list);
  std::list<IRCodePtr> collect();
  void allocate_registers();
 private:
  void divide_into_basic_blocks(std::list<IRCodePtr> ir_list);
  void link_basic_blocks();
  IRCodePtr header_;
  IRCodePtr footer_;
  std::vector<BasicBlockPtr> basic_block_vec_;
};

using FunctionBlockPtr = std::shared_ptr<FunctionBlock>;

FunctionBlockPtr new_function_block(std::list<IRCodePtr> &&ir_list) {
  return std::make_shared<FunctionBlock>(ir_list);
}

#endif //SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_
