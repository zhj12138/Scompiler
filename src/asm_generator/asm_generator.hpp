#ifndef SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_
#define SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_

#include "ir.hpp"

#include <string>
#include <vector>

class ASMGenerator {
 public:
  ASMGenerator() = default;
  ~ASMGenerator() = default;
  std::vector<std::string> generate(IRBuilderPtr &ir_builder);
 private:

};

std::vector<std::string> generate(IRBuilderPtr &ir_builder) {
  ASMGenerator generator;
  return generator.generate(ir_builder);
}

#endif //SCOMPILER_SRC_ASM_GENERATOR_ASM_GENERATOR_HPP_
