# Scompiler

编译器源代码位于`src`目录下，用于测试的C文件(被编译文件)位于`test`目录下。

## 依赖

使用`C++17`语法，部分代码依赖于`boost`库。

## 文法

```text
program
    : (function | declaration)*

function
    : type Identifier '(' parameter_list ')' compound_statement

type
    : 'int'

parameter_list
    : (type Identifier (',' type Identifier)*)?

compound_statement
    : '{' block_item* '}'

block_item
    : statement
    | declaration

statement
    : 'return' expression ';'
    | expression? ';'
    | 'if' '(' expression ')' statement ('else' statement)?
    | compound_statement
    | 'for' '(' expression? ';' expression? ';' expression? ')' statement
    | 'for' '(' declaration expression? ';' expression? ')' statement
    | 'while' '(' expression ')' statement
    | 'do' statement 'while' '(' expression ')' ';'
    | 'break' ';'
    | 'continue' ';'

declaration
    : type Identifier ('[' Integer ']')* ('=' expression)? ';'


expression_list
    : (expression (',' expression)*)?

expression
    : assignment

assignment
    : conditional
    | unary '=' expression


conditional
    : logical_or
    | logical_or '?' expression ':' conditional

logical_or
    : logical_and
    | logical_or '||' logical_and

logical_and
    : equality
    | logical_and '&&' equality

equality
    : relational
    | equality ('=='|'!=') relational

relational
    : additive
    | relational ('<'|'>'|'<='|'>=') additive

additive
    : multiplicative
    | additive ('+'|'-') multiplicative

multiplicative
    : unary
    | multiplicative ('*'|'/'|'%') unary

unary
    : postfix
    | ('-'|'~'|'!') unary

postfix
    : primary
    | Identifier '(' expression_list ')'
    | postfix '[' expression ']'

primary
    : Integer
    | '(' expression ')'
    | Identifier
```

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

> 我并不打算支持字符串，并且只打算支持`int`和`int []`。

### TokenStream

我使用TokenStream类来封装token流，该类封装了一些用于语法分析的便捷方法：

```c++
void advance(int dis = 1);
bool is(TokenType type);
void consume(TokenType type);
std::string consume_identifier();
int consume_number();
bool look_ahead(int dis, TokenType type);
bool eof();
```

## 语法分析器：Parser

AST节点的定义位于`src/base/ast.hpp`中。

Parser是使用Visitor模式实现的递归下降分析器。

其中Visitor模式通过C++的函数重载实现(定义在`src/base/visitor.hpp`)：

```c++
class ASTVisitor {
public:
    virtual void visit(ProgramPtr &program) = 0;
    virtual void visit(FunctionPtr &function) = 0;
    virtual void visit(BaseType &type) = 0;
    virtual void visit(ParameterListPtr &parameter_list) = 0;
    virtual void visit(CompoundStatementPtr &compound_statement) = 0;
    virtual void visit(BlockItemPtr &block_item) = 0;
    virtual void visit(StatementPtr &statement) = 0;
    virtual void visit(DeclarationPtr &declaration) = 0;
    virtual void visit(ExpressionListPtr &expression_list) = 0;
    virtual void visit(ExpressionPtr &expression) = 0;
    virtual void visit(AssignmentPtr &assignment) = 0;
    virtual void visit(ConditionalPtr &conditional) = 0;
    virtual void visit(LogicalOrPtr &logical_or) = 0;
    virtual void visit(LogicalAndPtr &logical_and) = 0;
    virtual void visit(EqualityPtr &equality) = 0;
    virtual void visit(RelationalPtr &relational) = 0;
    virtual void visit(AdditivePtr &additive) = 0;
    virtual void visit(MultiplicativePtr &multiplicative) = 0;
    virtual void visit(UnaryPtr &unary) = 0;
    virtual void visit(PostfixPtr &postfix) = 0;
    virtual void visit(PrimaryPtr &primary) = 0;
};
```

节点类型基本与文法定义相同。

### ASTPrinter

一个辅助debug的工具，同样继承自`ASTVisitor`，可以打印出如下的一颗语法树：

```text
program
    └── function(func)
        ├── type: int
        ├── parameterList
        │    └── variable(name: a, type: int)
        └── compoundStatement
            ├── blockItem
            │    └── declaration
            │        └── variable(name: b, type: int[5][7][9])
            ├── blockItem
            │    └── declaration
            │        ├── variable(name: c, type: int)
            │        └── expression
            │            └── assignment
            │                └── conditional
            │                    └── logicalOr
            │                        └── logicalAnd
            │                            └── equality
            │                                └── relational
            │                                    └── additive
            │                                        └── multiplicative
            │                                            └── unary
            │                                                └── postfix
            │                                                    └── primary(Identifier: b)
            └── blockItem
                └── statement
                    └── returnStatement
                        └── expression
                            └── assignment
                                └── conditional
                                    └── logicalOr
                                        └── logicalAnd
                                            └── equality
                                                └── relational
                                                    └── additive
                                                        └── multiplicative
                                                            └── unary
                                                                └── postfix
                                                                    └── primary(Interger: 0)

```

## 语义检查：Checker

目前只支持非常简单的代码检查，只检查函数和变量是否重复定义，变量是否未经声明就使用，函数调用参数是否合法。

后面还会增加一些更加复杂的检查，如类型检查，左值检查等。

这个步骤其实是可以和后面的语法制导翻译合并在一起的，但是因为后面的语义检查会比较复杂，所以我现在把它单独提取出来作为一个阶段。

Checker同样使用Visitor模式实现。

语义检查需要借助符号表，符号表的定义位于`src/base/symbol_table.hpp`中。

## 语法制导翻译：Translator

### 中间代码：IR

IR定义位于`src/base/ir.hpp`中，支持如下IR指令：

```text
 str代表字符串类型，imm代表数字类型，var代表变量类型, null代表不使用
 Op       a0      a1      a2      作用
 FUNBEG   str     null    null    函数a0开始
 FUNEND   null    null    null    函数结束
 LABEL    imm     null    null    根据a0生成一个唯一的标签
 RET      var|imm null    null    返回a0
 MOV      var     var|imm null    a0 = a1
 NEG      var     var     null    a0 = -a1
 NOT      var     var     null    a0 = ~a1
 LNOT     var     var     null    a0 = !a1
 MUL      var     var|imm var|imm a0 = a1 * a2
 DIV      var     var|imm var|imm a0 = a1 / a2
 REM      var     var|imm var|imm a0 = a1 % a2
 ADD      var     var|imm var|imm a0 = a1 + a2
 SUB      var     var|imm var|imm a0 = a1 - a2
 LT       var     var|imm var|imm a0 = a1 < a2
 GT       var     var|imm var|imm a0 = a1 > a2
 LE       var     var|imm var|imm a0 = (a1 <= a2)
 GE       var     var|imm var|imm a0 = (a1 >= a2)
 EQ       var     var|imm var|imm a0 = (a1 == a2)
 NE       var     var|imm var|imm a0 = (a1 != a2)
 LAND     var     var|imm var|imm a0 = (a1 && a2)
 LOR      var     var|imm var|imm a0 = (a1 || a2)
 JMP      imm     null    null    跳转到a0对应的标签
 BEQZ     var|imm imm     null    如果a0 == 0,则跳转到a1对应的标签
 PARAM    var     null    null    将a0作为参数进行传递
 CALL     var     str     null    调用函数a1，将返回值存放在a0中
 LA       var     str     null    将全局变量a1的地址加载到a0中
 LOAD     var     var     var|imm 以a1为基地址，a2为偏移地址的对应地址的值加载到a0中
 STORE    var     var     var|imm 将a0的值存放到以a1为基地址，a2为偏移地址的对应地址
 ALLOC    var     imm     null    为局部数组分配a1大小的空间，返回首地址到var中
 GBSS     str     imm     null    为全局变量a0分配a1大小的空间(可能是数组)
 GINI     str     imm     null    为全局变量a0分配内存，并初始化为imm(不为数组)
```

## 优化器：Optimizer

目前无任何功能

## 寄存器分配：RegAllocator

目前暂未实现

## 目标代码生成：ASMGenerator

使用RiscV汇编，因为寄存器分配暂未实现，所以该部分暂时无法完成(其实也可以实现一个纯栈式的生成器，不过那样目标代码执行效率就会很低了)。

