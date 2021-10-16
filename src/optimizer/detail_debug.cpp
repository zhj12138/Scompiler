#include "detail_debug.hpp"

#include "debug.hpp"

std::ostream &operator<<(std::ostream &os, const BasicBlock &basic_block) {
  for (auto &ir : basic_block.ir_list_) {
    os << *ir << std::endl;
  }
  return os;
}


template<typename T>
inline void PRINT_ELEMENTS(const T &coll,
                           const std::string &prompt = "",
                           const std::string &sep = " ",
                           const std::string &endstr = "\n") {
  std::cout << prompt;
  for (auto &elem: coll) {
    std::cout << elem << sep;
  }
  std::cout << endstr;
}

void PRINT_PRED_SUCC_BLOCKS(const std::list<WeakBasicBlockPtr> &l) {
  for (const auto &elem: l) {
    std::cout << (elem.lock())->block_num_ << " ";
  }
  std::cout << std::endl;
}

std::ostream &operator<<(std::ostream &os, FunctionBlock &function_block) {
  std::cout << green << function_block.func_name() << ": " << normal << std::endl;
  std::cout << "----------------------------------" << std::endl;
  function_block.live_variable_analysis();
  for (const auto &basic_block : function_block.basic_block_vec()) {
    os << red << "block " << basic_block->block_num_ << ":" << normal << std::endl;
    os << *basic_block;
    os << blue << "predecessor: " << normal;
    PRINT_PRED_SUCC_BLOCKS(basic_block->predecessor_list_);
    os << blue << "successor: " << normal;
    PRINT_PRED_SUCC_BLOCKS(basic_block->successor_list_);
    os << blue << "use: " << normal;
    PRINT_ELEMENTS(basic_block->use_);
    os << blue << "def: " << normal;
    PRINT_ELEMENTS(basic_block->def_);
    os << blue << "live_variable_IN_: " << normal;
    PRINT_ELEMENTS(basic_block->live_variable_IN_);
    os << blue << "live_variable_OUT_: " << normal;
    PRINT_ELEMENTS(basic_block->live_variable_OUT_);
    os << std::endl;
  }
  std::cout << "----------------------------------" << std::endl;
  return os;
}

std::ostream &operator<<(std::ostream &os, const Module &module) {
  std::cout << yellow << "global_declarations: " << normal << std::endl;
  for (auto &ir : module.global_ir_list()) {
    std::cout << *ir << std::endl;
  }
  std::cout << yellow << "functions: " << normal << std::endl;
  for (auto &func : module.function_vec()) {
    os << *func;
  }
  return os;
}
