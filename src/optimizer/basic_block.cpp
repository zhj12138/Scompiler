#include "basic_block.hpp"

void BasicBlock::calc_use_def() {
  use_.clear();
  def_.clear();
  // 忽略全局变量(TODO: 那么直接用int表示?)
  auto add_to_use = [&](const IRAddrPtr &addr) {
    if (addr->is_var()) {
      if (addr->var().is_global()) return;
      use_.insert(addr->var());
      def_.erase(addr->var());
    }
  };
  auto add_to_def = [&](const IRAddrPtr &addr) {
    if (addr->is_var()) {
      if (addr->var().is_global()) return;
      def_.insert(addr->var());
      use_.erase(addr->var());
    }
  };
  for (auto it = ir_list_.rbegin(); it != ir_list_.rend(); ++it) {
    auto cur_ir = *it;
    if (cur_ir->op() == IROp::RET ||
        cur_ir->op() == IROp::BEQZ ||
        cur_ir->op() == IROp::PARAM) {
      add_to_use(cur_ir->a0());
    } else if (cur_ir->op() == IROp::MOV ||
        is_unary_op(cur_ir->op())) {
      add_to_def(cur_ir->a0());
      add_to_use(cur_ir->a1());
    } else if (is_binary_op(cur_ir->op()) ||
        cur_ir->op() == IROp::LOAD) {
      add_to_def(cur_ir->a0());
      add_to_use(cur_ir->a1());
      add_to_use(cur_ir->a2());
    } else if (cur_ir->op() == IROp::CALL ||
        cur_ir->op() == IROp::LA ||
        cur_ir->op() == IROp::ALLOC) {
      add_to_def(cur_ir->a0());
    } else if (cur_ir->op() == IROp::STORE) {
      add_to_use(cur_ir->a0());
      add_to_use(cur_ir->a1());
      add_to_use(cur_ir->a2());
    } // 忽略其他的情况
  }
}
