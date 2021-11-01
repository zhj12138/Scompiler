# Scompiler

## 项目结构

项目目录下有如下文件夹

* src：编译器源代码。
  * base: 一些基本的类的定义。
  * lexer: 词法分析器。
  * parser: 语法分析器。
  * checker: 语义检查器。
  * translator: 语法制导翻译器。
  * optimizer: 优化器。
  * asm_generator: 目标代码生成器
* test: 用于测试的C文件(被编译代码)。
* output: 编译器的输出。
  * token.txt: 所有token。
  * ast.txt: 语法树。
  * ir.txt: 生成的IR代码。
  * low_ir.txt: 优化后的IR代码。  
  * asm.txt: 生成的目标代码。

> 注：我并未打印出符号表，因为我的符号表是动态生成的，离开作用域后会被清空。
> 
> 注：示例代码test目录下的lex.c文件都是有语法错误的(使用了不合法的16进制常数)，其余文件应该都可正常分析
> 
> 注：为了方便其他人测试，我将自己电脑上编译出来的Scompiler二进制文件复制到了build目录下，不过我并没有测试是否能在未安装Boost库的电脑上运行，不过我猜测大概率可以。

## 依赖

使用`C++17`语法，部分代码依赖于`boost`库。

## 命令行参数

```shell
Usage: Scompiler <input-file>
Options:
  -h [ --help ]            print help message
  --input-file arg         the source c file
  -t [ --token-file ] arg  file to store token stream
  -a [ --ast-file ] arg    file to store ast tree
  -i [ --ir-file ] arg     file to store ir code
  -l [ --low-ir-file ] arg file to store low ir code(after optimized and reg 
                           allocated)
  -o [ --output-file ] arg file to store asm code
  -O [ --optimize ] arg    optimize level
```

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

一个辅助debug的工具，同样继承自`ASTVisitor`，可以打印出类似下方的一颗语法树：

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

后面可能还会增加一些更加复杂的检查，如类型检查，左值检查等。

这个步骤其实是可以和后面的语法制导翻译合并在一起的，但是因为后面的语义检查会比较复杂，所以我现在把它单独提取出来作为一个阶段。

Checker同样使用Visitor模式实现。

语义检查需要借助符号表，符号表的定义位于`src/base/symbol_table.hpp`中。

## 语法制导翻译：Translator

### 中间代码：IR

IR定义位于`src/base/ir.hpp`中，支持如下IR指令：

```text
 str代表字符串类型，imm代表数字类型，var代表变量类型, null代表不使用
 Op       a0      a1      a2      作用
 FUNBEG   str     imm     null    函数a0开始, 共有a1个参数
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

以下指令只会在寄存器分配后使用: 

```text
FUNBEG   str     imm     imm     相比原来的FUNBEG，添加a2，表示局部数组的开始地址, a1更新为fp-sp的大小
LOADFP   var     imm     null    以a1为偏移地址, fp寄存器为基址的地址的值加载到寄存器a0中
STOREFP  var     imm     null    将寄存器a0的值存放到以a1为偏移地址, fp寄存器为基址的地址中
LARRAY   var     imm     null    结合新的FUNBEG中的a2一起翻译，a1为相对偏移，将局部数组的首地址加载到a0中, 由ALLOC指令转换而来
```

## 优化器：Optimizer

目前只通过基于基本块的活跃变量分析进行了简单的寄存器分配，并未做任何其他优化。

首先定义了三个类`Module`, `FunctionBlock`, `BasicBlock`分别表示编译单元，函数块，基本块。

我在这几个类的构造函数中完成了基本块的划分。

活跃变量分析通过`FunctionBlock.live_variable_analysis()`函数实现，寄存器分配通过`FunctionBlock.allocate_registers()`函数实现。

我在每次执行完一条语句之后，会将所有不再活跃的变量从寄存器中清空。如果仍然出现寄存器不足的情况，会将变量溢出到栈中，下次使用的时候会生成将其从栈中加载出来的代码，以及写入栈中的代码。

其中寄存器分配的栈帧空间分配如下(`base/alloc_info.hpp`)：

```text
/*
 * ----------------------  高地址
 * 之前函数的栈帧
 *
 * (最下面是传给函数的参数)
 * ----------------------  <- fp(x8)
 * callee-saved寄存器保存区
 * ----------------------
 * 溢出变量保存区
 * ----------------------
 * 局部数组保存区
 * ----------------------  <- sp(x2)
 *                         低地址
 * 注：函数参数暂时不通过寄存器传递，只通过栈传递
 * */
```

## 目标代码生成：ASMGenerator

使用RiscV汇编，目标代码生成的过程整体比较简单，实现位于`ASMGenerator.generate()`函数中(`asm_generator/asm_generator.cpp`)。

