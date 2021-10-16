#ifndef SCOMPILER_SRC_OPTIMIZER_DETAIL_DEBUG_HPP_
#define SCOMPILER_SRC_OPTIMIZER_DETAIL_DEBUG_HPP_

#include "module.hpp"

std::ostream &operator<<(std::ostream &os, const BasicBlock &basic_block);
std::ostream &operator<<(std::ostream &os, FunctionBlock &function_block);
std::ostream &operator<<(std::ostream &os, const Module &module);

#endif //SCOMPILER_SRC_OPTIMIZER_DETAIL_DEBUG_HPP_
