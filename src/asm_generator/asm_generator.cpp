#include "asm_generator.hpp"

#include "string_util.hpp"

#include <cassert>

std::vector<std::string> ASMGenerator::generate(IRBuilderPtr &ir_builder) {
  std::vector<std::string> asmcode_vec;
  for (auto &ir : ir_builder->ircode_list()) {
    switch (ir->op()) {
      case IRCode::Op::FUNCBEG: {
        asmcode_vec.push_back(build_string("\t.text"));
        asmcode_vec.push_back(build_string("\t.global ", ir->a0()->name()));
        break;
      }
      case IRCode::Op::FUNCEND: break;
      case IRCode::Op::RET: {
        if (ir->a0()->is_var()) {
          // 暂时不处理
        } else if (ir->a0()->is_imm()) {
          asmcode_vec.push_back(build_string("\tli "));
        } else { assert(false); }
        break;
      }
      case IRCode::Op::MOV: {
        break;
      }
      case IRCode::Op::NEG: {
        break;
      }
      case IRCode::Op::NOT: {
        break;
      }
      case IRCode::Op::LNOT: {
        break;
      }
    }
  }
  return asmcode_vec;
}
