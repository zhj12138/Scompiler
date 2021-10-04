#ifndef SCOMPILER_SRC_UTIL_STRING_UTIL_HPP_
#define SCOMPILER_SRC_UTIL_STRING_UTIL_HPP_

#include <string>
#include <sstream>

template<typename... Args>
std::string build_string(const Args &...args) {
  std::stringstream ss;
  (ss << ... << args);
  return ss.str();
}

#endif //SCOMPILER_SRC_UTIL_STRING_UTIL_HPP_
