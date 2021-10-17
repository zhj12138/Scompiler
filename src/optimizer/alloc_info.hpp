#ifndef SCOMPILER_SRC_OPTIMIZER_ALLOC_INFO_HPP_
#define SCOMPILER_SRC_OPTIMIZER_ALLOC_INFO_HPP_

#include "variable.hpp"

#include <map>
#include <set>

inline const int reg_x0 = 0;  // 始终是0
inline const int reg_sp = 2;
inline const int reg_t0 = 5;
inline const int reg_t1 = 6;
inline const int reg_fp = 8;

class Register {
 public:
  explicit Register(int num) : register_num_(num), used_(false) {}
  ~Register() = default;
  [[nodiscard]] int register_num() const { return register_num_; }
  [[nodiscard]] bool used() const { return used_; }
  void bind(const IRVar &var) {
    used_ = true;
    var_ = var;
  }
  void clear() { used_ = false; }
  [[nodiscard]] const IRVar &var() const { return var_; }
 private:
  int register_num_; // 寄存器号
  bool used_;  // 是否被使用
  IRVar var_;  // 寄存器关联的变量
  // 也许可以记录一下使用的次数，进而使得寄存器分配效果更好
};

/*
 * ----------------------  高地址
 * 之前函数的栈帧
 *
 * (最下面是传给函数的参数)
 * ----------------------  <- fp(x8)
 * callee-saved寄存器保存区
 * ----------------------
 * 溢出变量保存区
 * ----------------------
 * 局部数组保存区
 * ----------------------  <- sp(x2)
 *                         低地址
 * 注：函数参数暂时不通过寄存器传递，只通过栈传递
 * */
class AllocInfo {
 public:
  AllocInfo() {
    for (int i = 0; i < 32; ++i) {
      if (i == reg_x0 || i == reg_fp || i == reg_sp || i == reg_t0 || i == reg_t1) continue; // 忽略x0, sp和fp
      registers_.emplace_back(i);
    }
  }
  ~AllocInfo() = default;
  [[nodiscard]] int find_var_in_reg(const IRVar &var) const; // 从寄存器中寻找变量，找到了返回寄存器号，未找到返回-1
  [[nodiscard]] bool has_free_register() const; // 有空闲的寄存器
  int alloc_for_var(const IRVar &var);  // 为变量分配寄存器，并返回分配的寄存器号
  [[nodiscard]] int find_var_in_stack(const IRVar &var) const; // 从栈中查找变量，返回相对fp的偏移地址
  void clear_dead_reg(const std::set<IRVar> &live_variables);
  std::pair<int, int> spill_var(const IRVar &new_var);
  int alloc_tmp_reg() {
    if (!t0_used_) {
      t0_used_ = true;
      return reg_t0;
    }
    if (!t1_used_) {
      t1_used_ = true;
      return reg_t1;
    }
    assert(false);
  }
  void reset_tmp_regs() {
    t0_used_ = false;
    t1_used_ = false;
  }
  int alloc_for_array(int size) {
    int old_size = array_size_;
    array_size_ += size;
    return old_size;
  }
 private:
  static const int kSavedRegisterSize = 8;  // 只需保存fp寄存器和sp寄存器, 一个寄存器4字节
  std::vector<Register> registers_;
  int spill_size_{0};
  int array_size_{0};
  std::map<IRVar, int> spill_map_;
  bool t0_used_{false};
  bool t1_used_{false};
};

#endif //SCOMPILER_SRC_OPTIMIZER_ALLOC_INFO_HPP_
