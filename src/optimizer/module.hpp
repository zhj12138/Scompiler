#ifndef SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_
#define SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_

#include "function_block.hpp"

class Module {
 public:
  explicit Module(std::list<IRCodePtr> ir_list);
  void optimize(int optimize_level);
  void allocate_registers();
  std::list<IRCodePtr> collect();
  [[nodiscard]] const std::vector<FunctionBlockPtr> &function_vec() const { return function_vec_; }
  [[nodiscard]] const std::list<IRCodePtr> &global_ir_list() const { return global_ir_list_; }
 private:
  std::vector<FunctionBlockPtr> function_vec_;
  std::list<IRCodePtr> global_ir_list_;
};

#endif //SCOMPILER_SRC_OPTIMIZER_MODULE_HPP_
