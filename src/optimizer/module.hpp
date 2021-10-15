#ifndef SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_
#define SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_

#include "function_block.hpp"

class Module {
 public:
  explicit Module(std::list<IRCodePtr> ir_list);
  void optimize(int optimize_level);
  void allocate_registers();
  std::list<IRCodePtr> collect();
 private:
  std::vector<FunctionBlockPtr> function_vec_;
  std::list<IRCodePtr> global_ir_list_;
};

#endif //SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_
