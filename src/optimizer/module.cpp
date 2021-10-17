#include "module.hpp"

Module::Module(std::list<IRCodePtr> ir_list) {
  while (!ir_list.empty()) {
    while (!ir_list.empty() && ir_list.front()->op() != IROp::FUNBEG) { // 全局声明
      global_ir_list_.splice(global_ir_list_.end(), ir_list, ir_list.begin());
    }
    if (ir_list.empty()) break;
    auto iter = ir_list.begin();
    while ((*iter)->op() != IROp::FUNEND) ++iter;
    ++iter; // 跳到FUNEND的下一句
    std::list<IRCodePtr> tmp_ir_list;
    tmp_ir_list.splice(tmp_ir_list.end(), ir_list, ir_list.begin(), iter);
    function_vec_.push_back(new_function_block(std::move(tmp_ir_list)));
  }
}

std::list<IRCodePtr> Module::collect() {
  std::list<IRCodePtr> ir_list;
  ir_list.splice(ir_list.end(), global_ir_list_);
  for (auto &function : function_vec_) {
    ir_list.splice(ir_list.end(), function->collect());
  }
  return ir_list;
}

void Module::optimize(int optimize_level) {
  // do nothing now
}

void Module::allocate_registers() {
  for (auto &func : function_vec_) {
    func->allocate_registers();
  }
}
