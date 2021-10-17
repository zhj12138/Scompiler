#include "alloc_info.hpp"

#include <algorithm>

int AllocInfo::find_var_in_reg(const IRVar &var) const {
  for (const auto &reg : registers_) {
    if (reg.used() && reg.var() == var) return reg.register_num();
  }
  return -1;  // 未找到
}

bool AllocInfo::has_free_register() const {
  return std::any_of(registers_.begin(), registers_.end(), [](const Register &reg) {
    return !reg.used();
  });
}

int AllocInfo::alloc_for_var(const IRVar &var) {
  for (auto &reg : registers_) {
    if (!reg.used()) {
      reg.bind(var);
      return reg.register_num();
    }
  }
  assert(false);
}

int AllocInfo::find_var_in_stack(const IRVar &var) const {
  auto result = spill_map_.find(var);
  assert(result != spill_map_.end());
  return result->second + kSavedRegisterSize;
}

void AllocInfo::clear_dead_reg(const std::set<IRVar> &live_variables) {
  for (auto &reg : registers_) {
    if (reg.used()) {
      if (!live_variables.count(reg.var())) { // 不在活跃变量中
        reg.clear();
      }
    }
  }
}

std::pair<int, int> AllocInfo::spill_var(const IRVar &new_var) {
  // TODO: 可以采取更好的溢出策略
  static int i = 0;
  ++i;
  IRVar var = registers_[i].var();
  spill_map_[var] = spill_size_;
  spill_size_ += 4;
  registers_[i].bind(new_var);
  if (var.is_param()) {
    return {var.num() * 4, i};
  }
  return {spill_size_ - 4 + kSavedRegisterSize, i};
}
