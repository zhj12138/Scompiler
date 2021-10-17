#include "asm_generator.hpp"

#include "alloc_info.hpp"
#include "string_util.hpp"

#include <cassert>

std::string op_to_asm(IROp op) {
  switch (op) {
    case IROp::RET: return "ret";
    case IROp::MOV: return "mv";
    case IROp::NEG: return "neg";
    case IROp::NOT: return "not";
    case IROp::LNOT: return "seqz";
    case IROp::MUL: return "mul";
    case IROp::DIV: return "div";
    case IROp::REM: return "rem";
    case IROp::ADD: return "add";
    case IROp::SUB: return "sub";
    case IROp::LT: return "slt";
    case IROp::GT: return "sgt";
    case IROp::JMP: return "j";
    case IROp::BEQZ: return "beqz";
    case IROp::CALL: return "call";
    case IROp::LA: return "la";
    case IROp::LOAD: return "lw";
    case IROp::STORE: return "sw";
    default: break;
  }
  assert(false);
}

std::vector<std::string> ASMGenerator::generate(const std::list<IRCodePtr> &ir_list) {
  std::vector<std::string> asmcode_vec;
  auto get_reg_num = [&](IRAddrPtr &addr, int default_reg) -> int { // 如果addr为imm，则使用default_reg作为加载imm
    if (addr->is_imm()) {
      asmcode_vec.push_back(build_string("\tli x", default_reg, ", ", addr->imm()));
      return default_reg;
    }
    return addr->var().num();
  };
  for (auto &ir : ir_list) {
    switch (ir->op()) {
      case IROp::FUNBEG: {
        cur_func_name_ = ir->a0()->name();
        cur_fp_sp_diff_ = ir->a1()->imm();
        cur_array_offset_ = ir->a2()->imm();
        asmcode_vec.push_back(build_string("\t.text"));
        asmcode_vec.push_back(build_string("\t.global ", cur_func_name_));
        asmcode_vec.push_back(build_string(cur_func_name_, ":"));
        // 生成prologue代码, 保存旧的ra和fp, 更新sp和fp
        asmcode_vec.push_back(build_string("\taddi sp, sp, ", -cur_fp_sp_diff_));
        asmcode_vec.push_back(build_string("\tsw ra, ", cur_fp_sp_diff_ - 4, "(sp)"));
        asmcode_vec.push_back(build_string("\tsw fp, ", cur_fp_sp_diff_ - 8, "(sp)"));
        asmcode_vec.push_back(build_string("\taddi fp, sp, ", cur_fp_sp_diff_));
        break;
      }
      case IROp::FUNEND: {
        // 生成epilogue代码
        asmcode_vec.push_back(build_string(cur_func_name_, "_epilogue:"));
        asmcode_vec.push_back(build_string("\tlw ra, ", cur_fp_sp_diff_ - 4, "(sp)"));
        asmcode_vec.push_back(build_string("\tlw fp, ", cur_fp_sp_diff_ - 8, "(sp)"));
        asmcode_vec.push_back(build_string("\taddi sp, sp,", cur_fp_sp_diff_));
        asmcode_vec.push_back(build_string("\tret"));
        break;
      }
      case IROp::RET: {
        if (ir->a0()->is_imm()) {
          asmcode_vec.push_back(build_string("\tli a0, ", ir->a0()->imm()));
        } else {  // 是一个寄存器号
          asmcode_vec.push_back(build_string("\tmv a0, x", ir->a0()->var().num()));
        }
        asmcode_vec.push_back(build_string("\tj ", cur_func_name_, "_epilogue"));
        break;
      }
      case IROp::MOV: {
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        asmcode_vec.push_back(build_string("\tmv x", ir->a0()->var().num(), " , x", a1_reg));
        break;
      }
      case IROp::NEG: {
        asmcode_vec.push_back(build_string("\tneg x", ir->a0()->var().num(), " ,x", ir->a1()->var().num()));
        break;
      }
      case IROp::NOT: {
        asmcode_vec.push_back(build_string("\tnot x", ir->a0()->var().num(), " ,x", ir->a1()->var().num()));
        break;
      }
      case IROp::LNOT: {
        asmcode_vec.push_back(build_string("\tseqz x", ir->a0()->var().num(), " ,x", ir->a1()->var().num()));
        break;
      }
      case IROp::LABEL: {
        asmcode_vec.push_back(build_string(".L", ir->a0()->imm()));
        break;
      }
      case IROp::MUL:
      case IROp::DIV:
      case IROp::REM:
      case IROp::ADD:
      case IROp::SUB:
      case IROp::LT:
      case IROp::GT: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\t", op_to_asm(ir->op()),
                                           " x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        break;
      }
      case IROp::LE: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\tsgt x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\txori x", a0_reg,
                                           ", x", a0_reg,
                                           ", 1"));
        break;
      }
      case IROp::GE: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\t slt x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\txori x", a0_reg,
                                           ", x", a0_reg,
                                           ", 1"));
        break;
      }
      case IROp::EQ: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\tsub x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\tseqz x", a0_reg,
                                           ", x", a0_reg));
        break;
      }
      case IROp::NE: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\tsub x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\tsnez x", a0_reg,
                                           ", x", a0_reg));
        break;
      }
      case IROp::LAND: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\tsnez x", a0_reg,
                                           ", x", a1_reg));
        asmcode_vec.push_back(build_string("\tsub x", a0_reg,
                                           ", x", 0,
                                           ", x", a0_reg));
        asmcode_vec.push_back(build_string("\tand x", a0_reg,
                                           ", x", a0_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\tsnez x", a0_reg,
                                           ", x", a0_reg));
        break;
      }
      case IROp::LOR: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = get_reg_num(ir->a1(), reg_t0);
        int a2_reg = get_reg_num(ir->a2(), reg_t1);
        asmcode_vec.push_back(build_string("\tor x", a0_reg,
                                           ", x", a1_reg,
                                           ", x", a2_reg));
        asmcode_vec.push_back(build_string("\tsnez x", a0_reg,
                                           ", x", a0_reg));
        break;
      }
      case IROp::JMP: {
        asmcode_vec.push_back(build_string("\tj .L", ir->a0()->imm()));
        break;
      }
      case IROp::BEQZ: {
        int a0_reg = get_reg_num(ir->a0(), reg_t0);
        asmcode_vec.push_back(build_string("\tbeqz x", a0_reg, ", .L", ir->a1()->imm()));
        break;
      }
      case IROp::PARAM: {
        asmcode_vec.push_back(build_string("\taddi sp, sp, -4"));
        asmcode_vec.push_back(build_string("\tsw x", ir->a0()->var().num(), ", 0(sp)"));
        ++cur_param_num_;
        break;
      }
      case IROp::CALL: {
        asmcode_vec.push_back(build_string("\tcall ", ir->a1()->name()));
        asmcode_vec.push_back(build_string("\tmv x", ir->a0()->var().num(), ", a0"));
        asmcode_vec.push_back(build_string("\taddi sp, sp, ", cur_param_num_ * 4));
        cur_param_num_ = 0;
        break;
      }
      case IROp::LA: {
        asmcode_vec.push_back(build_string("\tla x", ir->a0()->var().num(), ", ", ir->a1()->name()));
        break;
      }
      case IROp::LOAD: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = ir->a1()->var().num();
        if (ir->a2()->is_imm()) {
          asmcode_vec.push_back(build_string("\tlw x", a0_reg,
                                             ", ", ir->a2()->imm(),
                                             "(x", a1_reg, ")"));
        } else {  // 按照目前的中间代码翻译，else分支不会被执行
          int a2_reg = ir->a2()->var().num();
          asmcode_vec.push_back(build_string("\tadd x", reg_t0,
                                             ", x", a1_reg,
                                             ", x", a2_reg));
          asmcode_vec.push_back(build_string("\tlw x", a0_reg,
                                             ", 0(", reg_t0, ")"));
        }
        break;
      }
      case IROp::STORE: {
        int a0_reg = ir->a0()->var().num();
        int a1_reg = ir->a1()->var().num();
        if (ir->a2()->is_imm()) {
          asmcode_vec.push_back(build_string("\tsw x", a0_reg,
                                             ", ", ir->a2()->imm(),
                                             "(x", a1_reg, ")"));
        } else {  // 按照目前的中间代码翻译，else分支不会被执行
          int a2_reg = ir->a2()->var().num();
          asmcode_vec.push_back(build_string("\tadd x", reg_t0,
                                             ", x", a1_reg,
                                             ", x", a2_reg));
          asmcode_vec.push_back(build_string("\tsw x", a0_reg,
                                             ", 0(", reg_t0, ")"));
        }
        break;
      }
      case IROp::ALLOC: assert(false);
      case IROp::GBSS: {
        asmcode_vec.push_back(build_string(".bss"));
        asmcode_vec.push_back(build_string(".global ", ir->a0()->name()));
        asmcode_vec.push_back(build_string(ir->a0()->name(), ":"));
        asmcode_vec.push_back(build_string("\t.space ", ir->a1()->imm()));
        break;
      }
      case IROp::GINI: {
        asmcode_vec.push_back(build_string(".data"));
        asmcode_vec.push_back(build_string(".global ", ir->a0()->name()));
        asmcode_vec.push_back(build_string(ir->a0()->name(), ":"));
        asmcode_vec.push_back(build_string("\t.word ", ir->a1()->imm()));
        break;
      }
      case IROp::LOADFP: {
        int a0_reg = ir->a0()->var().num();
        asmcode_vec.push_back(build_string("\tlw x", a0_reg,
                                           ", ", ir->a1()->imm(), "(fp)"));
        break;
      }
      case IROp::STOREFP: {
        int a0_reg = ir->a0()->var().num();
        asmcode_vec.push_back(build_string("\tsw x", a0_reg,
                                           ", ", ir->a1()->imm(), "(fp)"));
        break;
      }
      case IROp::LARRAY: {
        int a0_reg = ir->a0()->var().num();
        asmcode_vec.push_back(build_string("\tlw x", a0_reg,
                                           ", ", -(ir->a1()->imm() + cur_array_offset_), "(fp)"));
        break;
      }
    }
  }
  return asmcode_vec;
}
