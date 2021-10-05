#include "asm_generator.hpp"

#include "string_util.hpp"

#include <cassert>

std::vector<std::string> ASMGenerator::generate(IRBuilderPtr &ir_builder) {
  std::vector<std::string> asmcode_vec;
  for (auto &ir : ir_builder->ircode_list()) {
    switch (ir->op()) {
      case IROp::FUNBEG: {
        asmcode_vec.push_back(build_string("\t.text"));
        asmcode_vec.push_back(build_string("\t.global ", ir->a0()->name()));
        break;
      }
      case IROp::FUNEND: break;
      case IROp::RET: {
        if (ir->a0()->is_var()) {
          // 暂时不处理
        } else if (ir->a0()->is_imm()) {
          asmcode_vec.push_back(build_string("\tli "));
        } else { assert(false); }
        break;
      }
      case IROp::MOV: {
        break;
      }
      case IROp::NEG: {
        break;
      }
      case IROp::NOT: {
        break;
      }
      case IROp::LNOT: {
        break;
      }
    }
  }
  return asmcode_vec;
}
