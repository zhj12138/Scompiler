#include "spirit_lexer.hpp"

#include "lex_error.hpp"

#include <fstream>
#include <boost/spirit/include/lex_lexertl.hpp>

namespace lex = boost::spirit::lex;

enum ID {
  ID_IDENTIFIER = 100,
  ID_NEWLINE,
  ID_SPACE,
  ID_COMMENT,
  ID_DECNUMBER,
  ID_HEXNUMBER,
  ID_OCTNUMBER,
  ID_LPAREN,
  ID_RPAREN,
  ID_LSBRACE,
  ID_RSBRACE,
  ID_RCBRACE,
  ID_LCBRACE,
  ID_SEMICOLON,
  ID_COMMA,
  ID_SUB,
  ID_ADD,
  ID_TIMES,
  ID_DIVIDE,
  ID_MOD,
  ID_NOT,
  ID_LNOT,
  ID_OR,
  ID_LOR,
  ID_AND,
  ID_LAND,
  ID_ASSIGN,
  ID_GREATER,
  ID_LESS,
  ID_EQUAL,
  ID_NEQUAL,
  ID_GE,
  ID_LE,
  ID_QUESTION,
  ID_COLON,
  ID_STRING,
  ID_UNKNOWN,
};

template<typename Lexer>
class TLexer : public lex::lexer<Lexer> {
 public:
  TLexer() {
    this->self.add
        (R"([a-zA-Z_][a-zA-Z0-9_]*)", ID_IDENTIFIER)
        (R"([\n\r]+)", ID_NEWLINE)
        (R"([ \t])", ID_SPACE)
        (R"(\/\/.*?\n)", ID_COMMENT)
        (R"(0[xX][0-9a-fA-F]+)", ID_HEXNUMBER)
        (R"(0[0-7]*)", ID_OCTNUMBER)
        (R"([1-9][0-9]*)", ID_DECNUMBER)
        (R"(\()", ID_LPAREN)
        (R"(\))", ID_RPAREN)
        (R"(\[)", ID_LSBRACE)
        (R"(\])", ID_RSBRACE)
        (R"(\{)", ID_LCBRACE)
        (R"(\})", ID_RCBRACE)
        (R"(;)", ID_SEMICOLON)
        (R"(,)", ID_COMMA)
        (R"(-)", ID_SUB)
        (R"(\+)", ID_ADD)
        (R"(\*)", ID_TIMES)
        (R"(\/)", ID_DIVIDE)
        (R"(%)", ID_MOD)
        (R"(~)", ID_NOT)
        (R"(!=)", ID_NEQUAL)
        (R"(!)", ID_LNOT)
        (R"(\|\|)", ID_LOR)
        (R"(\|)", ID_OR)
        (R"(&&)", ID_LAND)
        (R"(&)", ID_AND)
        (R"(==)", ID_EQUAL)
        (R"(>=)", ID_GE)
        (R"(<=)", ID_LE)
        (R"(<)", ID_LESS)
        (R"(>)", ID_GREATER)
        (R"(=)", ID_ASSIGN)
        (R"(\?)", ID_QUESTION)
        (R"(:)", ID_COLON)
        (R"(\".*?\")", ID_STRING)
        (R"(.)", ID_UNKNOWN);
  }
};

class Tokenizer {
 public:
  template<typename T>
  bool operator()(const T &t, std::vector<Token> &token_vec) const {
    std::string t_str(t.value().begin(), t.value().end());
    switch (t.id()) {
      case ID_IDENTIFIER: {
        token_vec.push_back(get_token_by_identifier(t_str));
        break;
      }
      case ID_NEWLINE:
      case ID_SPACE:
      case ID_COMMENT: break;
      case ID_HEXNUMBER: {
        int num = std::stoi(t_str, nullptr, 16);
        token_vec.emplace_back(TokenType::Number, num);
        break;
      }
      case ID_OCTNUMBER: {
        int num = std::stoi(t_str, nullptr, 8);
        token_vec.emplace_back(TokenType::Number, num);
        break;
      }
      case ID_DECNUMBER: {
        int num = std::stoi(t_str);
        token_vec.emplace_back(TokenType::Number, num);
        break;
      }
      case ID_LPAREN: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Lparen);
        break;
      }
      case ID_RPAREN: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Rparen);
        break;
      }
      case ID_LSBRACE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::LSbrace);
        break;
      }
      case ID_RSBRACE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::RSbrace);
        break;
      }
      case ID_LCBRACE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::LCbrace);
        break;
      }
      case ID_RCBRACE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::RCbrace);
        break;
      }
      case ID_SEMICOLON: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Semicolon);
        break;
      }
      case ID_COMMA: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Comma);
        break;
      }
      case ID_SUB: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Sub);
        break;
      }
      case ID_ADD: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::And);
        break;
      }
      case ID_TIMES: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Times);
        break;
      }
      case ID_DIVIDE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Divide);
        break;
      }
      case ID_MOD: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Mod);
        break;
      }
      case ID_NOT: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Not);
        break;
      }
      case ID_LNOT: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Lnot);
        break;
      }
      case ID_OR: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Or);
        break;
      }
      case ID_LOR: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Lor);
        break;
      }
      case ID_AND: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::And);
        break;
      }
      case ID_LAND: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Land);
        break;
      }
      case ID_ASSIGN: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Assign);
        break;
      }
      case ID_GREATER: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Greater);
        break;
      }
      case ID_LESS: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Less);
        break;
      }
      case ID_EQUAL: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Equal);
        break;
      }
      case ID_NEQUAL: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Nequal);
        break;
      }
      case ID_GE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Ge);
        break;
      }
      case ID_LE: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Le);
        break;
      }
      case ID_QUESTION: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Question);
        break;
      }
      case ID_COLON: {
        token_vec.emplace_back(TokenType::Punctuation, PunctuactionType::Colon);
        break;
      }
      case ID_STRING: {
        token_vec.emplace_back(TokenType::String, t_str);
        break;
      }
      case ID_UNKNOWN:
        std::cout << "ID_UNKNOWN: ";
      default:
        throw lex_error("unknown character: " + t_str);
    }
    return true;
  }
};

std::vector<Token> SpiritLexer::lex(const std::string &code) {
  std::vector<Token> token_vec;
  TLexer<lex::lexertl::lexer<>> tlexer;
  Tokenizer tokenizer;
  const char *first = code.c_str();
  const char *last = &first[code.size()];
  bool r = lex::tokenize(first, last,
                         tlexer,
                         [tokenizer, &token_vec](auto &&PH1) {
                           return tokenizer(std::forward<decltype(PH1)>(PH1), token_vec);
                         });
  assert(r);
  return token_vec;
}
std::vector<Token> SpiritLexer::lex_file(const fs::path &file) {
  std::ifstream ifs(file);
  std::stringstream ss;
  ss << ifs.rdbuf();
  return lex(ss.str());
}
