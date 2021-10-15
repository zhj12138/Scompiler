#ifndef SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_
#define SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_

#include "ir.hpp"
#include "module.hpp"

inline void optimize(IRBuilderPtr &ir_builder, int optimize_level) {
  Module module(ir_builder->ircode_list());
  module.optimize(optimize_level);
  module.allocate_registers();
  ir_builder->ircode_list() = module.collect();
}

#endif //SCOMPILER_SRC_OPTIMIZER_OPTIMIZER_HPP_
