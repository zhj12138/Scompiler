#ifndef SCOMPILER_SRC_BASE_DEBUG_HPP_
#define SCOMPILER_SRC_BASE_DEBUG_HPP_

#include "token_stream.hpp"
#include "ast.hpp"
#include "ir.hpp"

#include <algorithm>

std::ostream &operator<<(std::ostream &os, const Token &token);
std::ostream &operator<<(std::ostream &os, TokenStream &token_stream);
std::ostream &operator<<(std::ostream &os, VariableType &variable);
std::ostream &operator<<(std::ostream &os, Variable &variable);
std::ostream &operator<<(std::ostream &os, ProgramPtr &program);
std::ostream &operator<<(std::ostream &os, IRVar &var);
std::ostream &operator<<(std::ostream &os, IRAddr &addr);
std::ostream &operator<<(std::ostream &os, IRCode &code);
std::ostream &operator<<(std::ostream &os, IRBuilderPtr &ir_builder);

#endif //SCOMPILER_SRC_BASE_DEBUG_HPP_
