# Scompiler

编译器源代码位于`src`目录下，用于测试的C文件(被编译文件)位于`test`目录下。

## 依赖

使用`C++17`语法，部分代码依赖于`boost`库(不过都提供了不依赖于`boost`库的实现)。

## 词法分析器：Lexer

代码位于`src/lexer`目录下。一共提供两种不同实现的Lexer:

* `HandwrittenLexer`：手动解析的词法分析器。
* `SpiritLexer`：利用`Boost.Spirit`库实现的词法分析器。

两种Lexer提供相同的接口：

```c++
std::vector<Token> lex(const std::string &code);
std::vector<Token> lex_file(const fs::path &file);
```

可以通过`lexer.hpp`文件中的`using`语句选择不同的实现：

```c++
using Lexer = SpiritLexer;
```

### Token

Token类定义如下：

```c++
class Token {
  TokenType type_;
  std::variant<KeywordType, PunctuactionType, int, std::string> value_;
};
```

只保存类型和对应的值，并不保存`lineno`等信息。

使用`enum class`来表示类型，使用`std::variant`记录值。

Token共有5种类型：

* `Keyword`: 关键字
* `Punctuation`: 标点符号(分隔符和运算符)
* `Identifier`: 标识符
* `Number`: 数字
* `String`: 字符串

> 我并不打算支持字符串，并且只打算支持`int`, `int*`和`int []`。


