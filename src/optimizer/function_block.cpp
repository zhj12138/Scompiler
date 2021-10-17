#include "function_block.hpp"

#include "alloc_info.hpp"

#include <algorithm>

namespace detail {

template<typename T>
void set_union(std::set<T> &cont1, const std::set<T> &cont2) {
  for (auto it = cont2.begin(); it != cont2.end(); ++it) {
    cont1.insert(*it);
  }
}

template<typename T>
void set_difference(std::set<T> &cont1, const std::set<T> &cont2) {
  for (auto it = cont2.begin(); it != cont2.end(); ++it) {
    cont1.erase(*it);
  }
}

}

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
        auto target_block = find_label(last_ir->a1()->imm());
        if (next_block->block_num_ != target_block->block_num_) {
          basic_block->successor_list_.push_back(next_block);
          next_block->predecessor_list_.push_back(basic_block);
        } // 两个目标相同则只建立前驱-后继关系一次，不确保该情况会出现
        basic_block->successor_list_.push_back(target_block);
        target_block->predecessor_list_.push_back(basic_block);
      } else {
        auto target_block = find_label(last_ir->a1()->imm());
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
  func_name_ = ir_list.front()->a0()->name();
  ir_list.pop_front();  // pop FUNBEG
  ir_list.pop_back();  // POP FUNEND

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

void FunctionBlock::live_variable_analysis() {
  // 计算每个基本块的use和def集
  for (auto &basic_block : basic_block_vec_) {
    basic_block->calc_use_def();
  }
  // 计算每个基本块入口和出口的活跃变量
  // 清空入口活跃变量
  for (auto &basic_block : basic_block_vec_) {
    basic_block->live_variable_IN_.clear();
  }
  bool change = true;
  while (change) {
    change = false;
    for (auto &basic_block : basic_block_vec_) {
      std::set<IRVar> tmp;
      // OUT(Block) = Union(IN(SuccBlock) for SuccBlock in Block->successor_list)
      for (const auto &succ_block: basic_block->successor_list_) {
        detail::set_union(tmp, succ_block.lock()->live_variable_IN_);
      }
      basic_block->live_variable_OUT_ = tmp;
      // tmp_OUT = OUT(Block) - DEF(Block)
      detail::set_difference(tmp, basic_block->def_);
      // IN(Block) = Union(USE(Block), tmp_difference_set)
      detail::set_union(tmp, basic_block->use_);
      if (basic_block->live_variable_IN_ != tmp) {
        change = true;
        basic_block->live_variable_IN_.swap(tmp);
      }
    }
  }
  // 计算执行每一条IR语句之前的活跃变量
  for (auto &basic_block : basic_block_vec_) {
    basic_block->calc_live_variable();
  }
}

void FunctionBlock::allocate_registers() {
  live_variable_analysis();
  // 寄存器分配后，依旧沿用旧的IRVar类，只使用数字类型表示寄存器号
  AllocInfo alloc_info;
  auto alloc_for_read = [&](const IRAddrPtr &addr,
                            std::list<IRCodePtr> &ir_list,
                            std::list<IRCodePtr>::iterator &iter) {
    if (!addr->is_var()) return;  // 只处理变量
    int reg_num = alloc_info.find_var_in_reg(addr->var());
    if (reg_num == -1) {  // 当前未存放在寄存器中, 那么就必然存放在栈中
      int offset = 0;
      if (addr->var().is_param()) { // 函数参数
        offset = addr->var().num() * 4;
      } else {
        offset = alloc_info.find_var_in_stack(addr->var());
      }
      int tmp_reg = alloc_info.alloc_tmp_reg();
      ir_list.insert(iter, new_ir(IROp::LOADFP,
                                         new_ir_addr(IRVar(tmp_reg)),
                                         new_ir_addr(offset)));  // 将值加载到临时寄存器中
      addr->var().num() = tmp_reg;
    } else {  // 已经存放在寄存器中
      addr->var().num() = reg_num;
    }
  };
  auto alloc_for_write = [&](const IRAddrPtr &addr,
                             std::list<IRCodePtr> &ir_list,
                             std::list<IRCodePtr>::iterator &iter,
                             const std::set<IRVar> &live_variables) {
    if (!addr->is_var()) return;  // 只处理变量
    int reg_num = alloc_info.find_var_in_reg(addr->var());
    if (reg_num == -1) {  // 当前未存放在寄存器中
      alloc_info.clear_dead_reg(live_variables);
      if (alloc_info.has_free_register()) { // 有空闲的寄存器
        reg_num = alloc_info.alloc_for_var(addr->var());
        addr->var().num() = reg_num;
      } else {  // 没有空闲的寄存器, 需要溢出到栈中
        auto[offset, reg_num] = alloc_info.spill_var(addr->var());
        // spill_var已经处理了函数参数的情况
        // 把寄存器原来的值存放到栈中
        ir_list.insert(iter, new_ir(IROp::STOREFP, new_ir_addr(IRVar(reg_num)), new_ir_addr(offset)));
        addr->var().num() = reg_num;
      }
    } else {  // 已经存放在寄存器中
      addr->var().num() = reg_num;
    }
  };
  int num = 0;
  for (auto &basic_block : basic_block_vec_) {
    auto it = basic_block->ir_list_.begin();
    auto live_it = basic_block->live_variable_set_deq_.begin();
    for (; it != basic_block->ir_list_.end(); ++live_it, ++it) {
      ++num;
      auto cur_ir = *it;
      auto &live_variables = *live_it;
      IROp cur_op = cur_ir->op();
      if (cur_op == IROp::RET || cur_op == IROp::BEQZ || cur_op == IROp::PARAM) {
        alloc_for_read(cur_ir->a0(), basic_block->ir_list_, it);
      } else if (cur_op == IROp::MOV || is_unary_op(cur_op)) {
        alloc_for_read(cur_ir->a1(), basic_block->ir_list_, it);
        alloc_for_write(cur_ir->a0(), basic_block->ir_list_, it, live_variables);
      } else if (is_binary_op(cur_op) || cur_op == IROp::LOAD){
        alloc_for_read(cur_ir->a1(), basic_block->ir_list_, it);
        alloc_for_read(cur_ir->a2(), basic_block->ir_list_, it);
        alloc_for_write(cur_ir->a0(), basic_block->ir_list_, it, live_variables);
      } else if (cur_op == IROp::CALL || cur_op == IROp::LA) {
        alloc_for_write(cur_ir->a0(), basic_block->ir_list_, it, live_variables);
      } else if (cur_op == IROp::STORE) {
        alloc_for_read(cur_ir->a0(), basic_block->ir_list_, it);
        alloc_for_read(cur_ir->a1(), basic_block->ir_list_, it);
        alloc_for_read(cur_ir->a2(), basic_block->ir_list_, it);
      } else if (cur_op == IROp::ALLOC) {
        alloc_for_write(cur_ir->a0(), basic_block->ir_list_, it, live_variables);
        cur_ir->op() = IROp::LARRAY;
        cur_ir->a1()->imm() = alloc_info.alloc_for_array(cur_ir->a1()->imm());
      }// 忽略其他的情况
      alloc_info.reset_tmp_regs();
    }
  }
  header_->a2() = new_ir_addr(alloc_info.array_base_offset());
  header_->a1()->imm() = alloc_info.fp_sp_diff();
}


