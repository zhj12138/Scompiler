#include "function_block.hpp"

void FunctionBlock::divide_into_basic_blocks(std::list<IRCodePtr> ir_list) {
  basic_block_vec_.clear();
  auto tmp_basic_block = make_empty_basic_block();  // 第一条指令是首指令
  while (!(ir_list.empty())) {
    auto front = ir_list.front();
    if (front->op() == IROp::LABEL) {
      if (!(tmp_basic_block->ir_list_.empty())) {
        basic_block_vec_.push_back(tmp_basic_block);
        tmp_basic_block = make_empty_basic_block(); // 跳转指令的目标指令是首指令
      }
      tmp_basic_block->ir_list_.push_back(front);
    } else if (is_jmp_op(front->op()) || (front->op() == IROp::RET)) {
      tmp_basic_block->ir_list_.push_back(front);
      basic_block_vec_.push_back(tmp_basic_block);
      tmp_basic_block = make_empty_basic_block(); // 跳转指令的后一条指令是首指令
    } else {
      tmp_basic_block->ir_list_.push_back(front);
    }
    ir_list.pop_front();
  }
  if (!(tmp_basic_block->ir_list_.empty())) {
    basic_block_vec_.push_back(tmp_basic_block);
  }
  int block_num = -1;
  for (const auto &basic_block : basic_block_vec_) {
    basic_block->block_num_ = ++block_num;
  }
}

void FunctionBlock::link_basic_blocks() {
  // 计算前驱节点和后继节点
  auto find_label = [&](int label_num) {
    for (const auto &basic_block : basic_block_vec_) {
      auto first_ir = basic_block->ir_list_.front();
      if (first_ir->op() == IROp::LABEL && first_ir->a0()->imm() == label_num) {
        return basic_block;
      }
    }
    assert(false);
  };

  for (auto iter = basic_block_vec_.begin(); iter != basic_block_vec_.end(); ++iter) {
    auto basic_block = *iter;
    auto last_ir = basic_block->ir_list_.back();
    if (last_ir->op() == IROp::JMP) {  // 无条件跳转
      auto target_block = find_label(last_ir->a0()->imm());
      basic_block->successor_list_.push_back(target_block);
      target_block->predecessor_list_.push_back(basic_block);
    } else if (last_ir->op() == IROp::RET) { // 无条件跳转
      // do nothing
    } else if (is_conditional_jmp_op(last_ir->op())) {  // 条件跳转
      if (next(iter) != basic_block_vec_.end()) {
        auto next_block = *next(iter);
        auto target_block = find_label(last_ir->a0()->imm());
        if (next_block->block_num_ != target_block->block_num_) {
          basic_block->successor_list_.push_back(next_block);
          next_block->predecessor_list_.push_back(basic_block);
        } // 两个目标相同则只建立前驱-后继关系一次，不确保该情况会出现
        basic_block->successor_list_.push_back(target_block);
        target_block->predecessor_list_.push_back(basic_block);
      } else {
        auto target_block = find_label(last_ir->a0()->imm());
        basic_block->successor_list_.push_back(target_block);
        target_block->predecessor_list_.push_back(basic_block);
      }
    } else {
      if (next(iter) != basic_block_vec_.end()) {
        auto next_block = *next(iter);
        basic_block->successor_list_.push_back(next_block);
        next_block->predecessor_list_.push_back(basic_block);
      }
    }
  }
}

FunctionBlock::FunctionBlock(std::list<IRCodePtr> ir_list) {
  header_ = ir_list.front();
  footer_ = ir_list.back();
  ir_list.pop_front();  // pop FUNCDEF
  ir_list.pop_back();  // POP FUNCEND

  divide_into_basic_blocks(ir_list);
  link_basic_blocks();
}

std::list<IRCodePtr> FunctionBlock::collect() {
  std::list<IRCodePtr> ret;
  ret.push_back(header_);
  for (auto &basic_block : basic_block_vec_) {
    ret.splice(ret.end(), basic_block->ir_list_);
  }
  ret.push_back(footer_);
  return ret;
}

void FunctionBlock::allocate_registers() {

}

