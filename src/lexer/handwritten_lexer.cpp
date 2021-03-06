#include "handwritten_lexer.hpp"

#include "compile_error.hpp"
#include <cassert>
#include <fstream>

// only support '//'
bool skip_comment(const std::string &code, int &index) {
  int len = code.length();
  while (code[index] != '\n') {
    ++index;
    if (index == len) return false;
  }
  return true;
}

bool skip_space(const std::string &code, int &index) {
  int len = code.length();
  while (index < len && isspace(code[index])) {
    ++index;
  }
  if (index == len) return false;
  return true;
}

Token parse_string(const std::string &code, int &index) {
  int len = code.length();
  int i = index + 1;
  while (i < len && code[i] != '"') {
    ++i;
  }
  if (i == len) {
    throw lex_error("parse string error");
  }
  std::string s = code.substr(index, i - index + 1);
  index = i;
  return Token(TokenType::String, s);
}

Token parse_identifier(const std::string &code, int &index) {
  int len = code.length();
  int i = index + 1;
  while (i < len && (isalnum(code[i]) || code[i] == '_')) {
    ++i;
  }
  std::string id = code.substr(index, i - index);
  index = i - 1;
  return get_token_by_identifier(id);
}

Token parse_hex(const std::string &code, int &index) {
  auto to_num = [](char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    assert(false);
  };
  int len = code.length();
  int i = index + 2;
  if (i == len) {
    throw lex_error("invalid hex number");
  }
  int value = 0;
  while (i < len && isxdigit(code[i])) {
    value = value * 16 + to_num(code[i]);
    ++i;
  }
  index = i - 1;
  return Token(TokenType::Number, value);
}

Token parse_oct(const std::string &code, int &index) {
  auto is_oct_num = [] (char c) {
    return c >= '0' && c <= '7';
  };
  int len = code.length();
  int i = index + 1;
  int value = 0;
  while (i < len && is_oct_num(code[i])) {
    value = value * 8 + (code[i] - '0');
    ++i;
  }
  index = i - 1;
  return Token(TokenType::Number, value);
}

Token parse_dec(const std::string &code, int &index) {
  int len = code.length();
  int i = index;
  int value = 0;
  while (i < len && isdigit(code[i])) {
    value = value * 10 + (code[i] - '0');
    ++i;
  }
  index = i - 1;
  return Token(TokenType::Number, value);
}

Token parse_number(const std::string &code, int &index) {
  int len = code.length();
  if (code[index] == '0') {
    if ((index + 1 < len) && (code[index + 1] == 'x' || code[index + 1] == 'X')) {
      return parse_hex(code, index);
    }
    if (index + 1 < len) {
      return parse_oct(code, index);
    }
  }
  return parse_dec(code, index);
}

// no lineno
std::vector<Token> HandwrittenLexer::lex(const std::string &code) {
  std::vector<Token> token_vec;
  int index = 0, length = code.length();
  while (index < length) {
    if (!skip_space(code, index)) break;
    switch (code[index]) {
      case '(':
        token_vec.emplace_back(TokenType::Lparen, "(");
        break;
      case ')':
        token_vec.emplace_back(TokenType::Rparen, ")");
        break;
      case '[':
        token_vec.emplace_back(TokenType::LSbrace, "[");
        break;
      case ']':
        token_vec.emplace_back(TokenType::RSbrace, "]");
        break;
      case '{':
        token_vec.emplace_back(TokenType::LCbrace, "{");
        break;
      case '}':
        token_vec.emplace_back(TokenType::RCbrace, "}");
        break;
      case ';':
        token_vec.emplace_back(TokenType::Semicolon, ";");
        break;
      case ',':
        token_vec.emplace_back(TokenType::Comma, ",");
        break;
      case '-':
        token_vec.emplace_back(TokenType::Sub, "-");
        break;
      case '+':
        token_vec.emplace_back(TokenType::Add, "+");
        break;
      case '*':
        token_vec.emplace_back(TokenType::Times, "*");
        break;
      case '/':
        if ((index + 1 < length) && code[index + 1] == '/') {
          if (!skip_comment(code, index)) break;
        } else {
          token_vec.emplace_back(TokenType::Divide, "/");
        }
        break;
      case '%':
        token_vec.emplace_back(TokenType::Mod, "%");
        break;
      case '~':
        token_vec.emplace_back(TokenType::Not, "~");
        break;
      case '!':
        if ((index + 1 < length) && code[index + 1] == '=') {
          ++index;
          token_vec.emplace_back(TokenType::Nequal, "!=");
        } else {
          token_vec.emplace_back(TokenType::Lnot, "!");
        }
        break;
      case '|':
        if ((index + 1 < length) && code[index + 1] == '|') {
          ++index;
          token_vec.emplace_back(TokenType::Lor, "||");
        } else {
          token_vec.emplace_back(TokenType::Or, "|");
        }
        break;
      case '&':
        if ((index + 1 < length) && code[index + 1] == '&') {
          ++index;
          token_vec.emplace_back(TokenType::Land, "&&");
        } else {
          token_vec.emplace_back(TokenType::And, "&");
        }
        break;
      case '=':
        if ((index + 1 < length) && code[index + 1] == '=') {
          ++index;
          token_vec.emplace_back(TokenType::Equal, "==");
        } else {
          token_vec.emplace_back(TokenType::Assign, "=");
        }
        break;
      case '<':
        if ((index + 1 < length) && code[index + 1] == '=') {
          ++index;
          token_vec.emplace_back(TokenType::Le, "<=");
        } else {
          token_vec.emplace_back(TokenType::Less, "<");
        }
        break;
      case '>':
        if ((index + 1 < length) && code[index + 1] == '=') {
          ++index;
          token_vec.emplace_back(TokenType::Ge, ">=");
        } else {
          token_vec.emplace_back(TokenType::Greater, ">");
        }
        break;
      case '?':
        token_vec.emplace_back(TokenType::Question, "?");
        break;
      case ':':
        token_vec.emplace_back(TokenType::Colon, ":");
        break;
      case '"': {
        token_vec.push_back(parse_string(code, index));
        break;
      }
      default:
        if (isalpha(code[index]) || code[index] == '_') {
          token_vec.push_back(parse_identifier(code, index));
        } else if (isdigit(code[index])) {
          token_vec.push_back(parse_number(code, index));
        } else {
          std::string msg = std::string("unknown charactor: ") + code[index];
          throw lex_error(msg);
        }
    }
    ++index;
  }
  return token_vec;
}
std::vector<Token> HandwrittenLexer::lex_file(const fs::path &file) {
  std::ifstream ifs(file);
  std::stringstream ss;
  ss << ifs.rdbuf();
  return lex(ss.str());
}
