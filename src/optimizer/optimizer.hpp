#ifndef SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_
#define SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_

#include "ir.hpp"

class Optimizer {
 public:
  Optimizer() = default;
  ~Optimizer() = default;

  void optimize(IRBuilderPtr &ir_builder);
};

void optimize(IRBuilderPtr &ir_builder) {
  Optimizer optimizer;
  optimizer.optimize(ir_builder);
}

#endif //SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_
