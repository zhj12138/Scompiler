#ifndef SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_
#define SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_

#include "basic_block.hpp"
#include <vector>

class FunctionBlock {
 public:
  explicit FunctionBlock(std::list<IRCodePtr> ir_list);
  std::list<IRCodePtr> collect();
  void allocate_registers();
  [[nodiscard]] const std::vector<BasicBlockPtr> &basic_block_vec() const { return basic_block_vec_; }
  [[nodiscard]] const std::string &func_name() const { return func_name_; }
  void live_variable_analysis();
 private:
  void divide_into_basic_blocks(std::list<IRCodePtr> ir_list);
  void link_basic_blocks();
  IRCodePtr header_;
  IRCodePtr footer_;
  std::string func_name_;
  std::vector<BasicBlockPtr> basic_block_vec_;
};

using FunctionBlockPtr = std::shared_ptr<FunctionBlock>;

inline FunctionBlockPtr new_function_block(std::list<IRCodePtr> &&ir_list) {
  return std::make_shared<FunctionBlock>(ir_list);
}

#endif //SCOMPILER_SRC_OPTIMIZER_FUNCTION_BLOCK_HPP_
