#ifndef SCOMPILER_SRC_PARSER_PARSER_HPP_
#define SCOMPILER_SRC_PARSER_PARSER_HPP_

#include "recursive_descent_parser.hpp"

using Parser = RecursiveDescentParser;

ProgramPtr parse(TokenStream token_stream) {
  Parser parser(token_stream);
  return parser.parse();
}

#endif //SCOMPILER_SRC_PARSER_PARSER_HPP_
