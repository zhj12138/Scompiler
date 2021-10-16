#ifndef SCOMPILER_SRC_BASE_DEBUG_HPP_
#define SCOMPILER_SRC_BASE_DEBUG_HPP_

#include "token_stream.hpp"
#include "ast.hpp"
#include "ir.hpp"

#include <algorithm>

inline const std::string normal = "\033[0m";
inline const std::string black = "\033[0;30m";
inline const std::string red = "\033[0;31m";
inline const std::string green = "\033[0;32m";
inline const std::string yellow = "\033[0;33m";
inline const std::string blue = "\033[0;34m";
inline const std::string white = "\033[0;37m";

std::ostream &operator<<(std::ostream &os, const Token &token);
std::ostream &operator<<(std::ostream &os, TokenStream &token_stream);
std::ostream &operator<<(std::ostream &os, VariableType &variable);
std::ostream &operator<<(std::ostream &os, Variable &variable);
std::ostream &operator<<(std::ostream &os, ProgramPtr &program);
std::ostream &operator<<(std::ostream &os, const IRVar &var);
std::ostream &operator<<(std::ostream &os, IRAddr &addr);
std::ostream &operator<<(std::ostream &os, IRCode &code);
std::ostream &operator<<(std::ostream &os, IRBuilderPtr &ir_builder);

#endif //SCOMPILER_SRC_BASE_DEBUG_HPP_
