#ifndef SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_
#define SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_

#include "ir.hpp"

#include <string>
#include <vector>

class ASMGenerator {
 public:
  ASMGenerator() = default;
  ~ASMGenerator() = default;
  std::vector<std::string> generate(const std::list<IRCodePtr> &ir_list);
 private:
  std::string cur_func_name_;  // 当前正在翻译的函数
  int cur_fp_sp_diff_{0};      // 当前函数fp-sp的大小
  int cur_array_offset_{0};    // 当前函数局部数组开始地址距离fp的offset
  int cur_param_num_{0};
};

inline std::vector<std::string> generate(IRBuilderPtr &ir_builder) {
  ASMGenerator generator;
  return generator.generate(ir_builder->ircode_list());
}

#endif //SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_
