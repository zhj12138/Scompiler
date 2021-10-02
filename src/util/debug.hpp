#ifndef SCOMPILER_SRC_BASE_DEBUG_HPP_
#define SCOMPILER_SRC_BASE_DEBUG_HPP_

#include "token_stream.hpp"
#include "ast.hpp"

#include <algorithm>

std::ostream &operator<<(std::ostream &os, const Token &token);
std::ostream &operator<<(std::ostream &os, TokenStream &token_stream);
std::ostream &operator<<(std::ostream &os, Variable &variable);
std::ostream &operator<<(std::ostream &os, ProgramPtr &program);

#endif //SCOMPILER_SRC_BASE_DEBUG_HPP_
