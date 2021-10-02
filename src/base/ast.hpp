#ifndef SCOMPILER_SRC_BASE_AST_HPP_
#define SCOMPILER_SRC_BASE_AST_HPP_

#include "token.hpp"
#include "variable.hpp"
#include "support_type.hpp"

#include <utility>
#include <vector>
#include <memory>

class Expression;
using ExpressionPtr = std::shared_ptr<Expression>;

class Primary {
 public:
  using value_type = std::variant<int, ExpressionPtr, std::string>;
  explicit Primary(value_type v) : value_(std::move(v)) {}
  ~Primary() = default;

  value_type &value() { return value_; }
 private:
  value_type value_;
};
using PrimaryPtr = std::shared_ptr<Primary>;

class ExpressionList;
using ExpressionListPtr = std::shared_ptr<ExpressionList>;

class FuncCall {
 public:
  FuncCall(std::string &func_name, ExpressionListPtr exp_list)
      : func_name_(func_name), expression_list_(std::move(exp_list)) {}
  ~FuncCall() = default;

  std::string &func_name() { return func_name_; }
  ExpressionListPtr &expression_list() { return expression_list_; }
 private:
  std::string func_name_;
  ExpressionListPtr expression_list_;
};
using FuncCallPtr = std::shared_ptr<FuncCall>;

// 因为当前不支持指针，所以其实Array解析的时候postfix只能递归到primary,而不能递归到func call，不过为了符合文法，还是支持该语法
class Array {
 public:
  using value_type = std::variant<FuncCallPtr, PrimaryPtr>;
  Array(value_type name, std::vector<ExpressionPtr> expressions)
      : name_(std::move(name)),
        expression_vec_(std::move(expressions)) {}
  ~Array() = default;

  value_type &name() { return name_; }
  std::vector<ExpressionPtr> &expression_vec() { return expression_vec_; }
 private:
  value_type name_;
  std::vector<ExpressionPtr> expression_vec_;
};
using ArrayPtr = std::shared_ptr<Array>;

class Postfix {
 public:
  using value_type = std::variant<PrimaryPtr, FuncCallPtr, ArrayPtr>;
  explicit Postfix(value_type p) : value_(std::move(p)) {}
  ~Postfix() = default;

  value_type &value() { return value_; }
 private:
  value_type value_;
};
using PostfixPtr = std::shared_ptr<Postfix>;

class Unary {
  using UnaryPtr = std::shared_ptr<Unary>;
  using value_type = std::variant<PostfixPtr, UnaryPtr>;
 public:
  enum class Op {
    Sub,
    Not,
    Lnot,
  };
  explicit Unary(PostfixPtr value) : op_(Op::Sub), value_(std::move(value)) {}
  Unary(Op op, UnaryPtr value)
      : op_(op), value_(std::move(value)) {}
  ~Unary() = default;

  Op &op() { return op_; }
  value_type &value() { return value_; }
 private:
  Op op_;
  value_type value_;
};
using UnaryPtr = std::shared_ptr<Unary>;

class Multiplicative {
  using MultiplicativePtr = std::shared_ptr<Multiplicative>;
 public:
  enum class Op {
    Times,
    Divide,
    Mod,
  };
  explicit Multiplicative(UnaryPtr right) : left_(nullptr), op_(Op::Divide), right_(std::move(right)) {}
  Multiplicative(MultiplicativePtr left, Op op, UnaryPtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Multiplicative() = default;

  MultiplicativePtr &left() { return left_; }
  Op &op() { return op_; }
  UnaryPtr &right() { return right_; }
 private:
  MultiplicativePtr left_;
  Op op_;
  UnaryPtr right_;
};
using MultiplicativePtr = std::shared_ptr<Multiplicative>;

class Additive {
  using AdditivePtr = std::shared_ptr<Additive>;
 public:
  enum class Op {
    Add,
    Sub,
  };
  explicit Additive(MultiplicativePtr right) : left_(nullptr), op_(Op::Add), right_(std::move(right)) {}
  Additive(AdditivePtr left, Op op, MultiplicativePtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Additive() = default;

  AdditivePtr &left() { return left_; }
  Op &op() { return op_; }
  MultiplicativePtr &right() { return right_; }
 private:
  AdditivePtr left_;
  Op op_;
  MultiplicativePtr right_;
};
using AdditivePtr = std::shared_ptr<Additive>;

class Relational {
  using RelationalPtr = std::shared_ptr<Relational>;
 public:
  enum class Op {
    Less,
    Greater,
    Le,
    Ge,
  };
  explicit Relational(AdditivePtr right) : left_(nullptr), op_(Op::Ge), right_(std::move(right)) {}
  Relational(RelationalPtr left, Op op, AdditivePtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Relational() = default;

  RelationalPtr &left() { return left_; }
  Op &op() { return op_; }
  AdditivePtr &right() { return right_; }
 private:
  RelationalPtr left_;
  Op op_;
  AdditivePtr right_;
};
using RelationalPtr = std::shared_ptr<Relational>;

class Equality {
  using EqualityPtr = std::shared_ptr<Equality>;
 public:
  enum class Op {
    Equal,
    Nequal,
  };
  explicit Equality(RelationalPtr right) : left_(nullptr), op_(Op::Equal), right_(std::move(right)) {}
  Equality(EqualityPtr left, Op op, RelationalPtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Equality() = default;

  EqualityPtr &left() { return left_; }
  Op &op() { return op_; }
  RelationalPtr &right() { return right_; }
 private:
  EqualityPtr left_;
  Op op_;
  RelationalPtr right_;
};
using EqualityPtr = std::shared_ptr<Equality>;

class LogicalAnd {
  using LogicalAndPtr = std::shared_ptr<LogicalAnd>;
 public:
  explicit LogicalAnd(EqualityPtr right) : left_(nullptr), right_(std::move(right)) {}
  LogicalAnd(LogicalAndPtr left, EqualityPtr right)
      : left_(std::move(left)), right_(std::move(right)) {}
  ~LogicalAnd() = default;

  LogicalAndPtr &left() { return left_; }
  EqualityPtr &right() { return right_; }
 private:
  LogicalAndPtr left_;
  EqualityPtr right_;
};
using LogicalAndPtr = std::shared_ptr<LogicalAnd>;

class LogicalOr {
  using LogicalOrPtr = std::shared_ptr<LogicalOr>;
 public:
  explicit LogicalOr(LogicalAndPtr right) : left_(nullptr), right_(std::move(right)) {}
  LogicalOr(LogicalOrPtr left, LogicalAndPtr right)
      : left_(std::move(left)), right_(std::move(right)) {}
  ~LogicalOr() = default;

  LogicalOrPtr &left() { return left_; }
  LogicalAndPtr &right() { return right_; }
 private:
  LogicalOrPtr left_; // nullptr for only logical_and
  LogicalAndPtr right_;
};
using LogicalOrPtr = std::shared_ptr<LogicalOr>;

class Conditional {
  using ConditionalPtr = std::shared_ptr<Conditional>;
 public:
  Conditional(LogicalOrPtr cond, ExpressionPtr cond_true, ConditionalPtr cond_false)
      : cond_(std::move(cond)), cond_true_(std::move(cond_true)), cond_false_(std::move(cond_false)) {}
  ~Conditional() = default;
  LogicalOrPtr &cond() { return cond_; }
  ExpressionPtr &cond_true() { return cond_true_; }
  ConditionalPtr &cond_false() { return cond_false_; }
 private:
  LogicalOrPtr cond_;
  ExpressionPtr cond_true_;
  ConditionalPtr cond_false_;
};
using ConditionalPtr = std::shared_ptr<Conditional>;

// unary '=' expression
class AssignExp {
 public:
  AssignExp(UnaryPtr left, ExpressionPtr right)
      : left_(std::move(left)), right_(std::move(right)) {}
  ~AssignExp() = default;

  UnaryPtr &left() { return left_; }
  ExpressionPtr &right() { return right_; }
 private:
  UnaryPtr left_;
  ExpressionPtr right_;
};
using AssignExpPtr = std::shared_ptr<AssignExp>;

class Assignment {
 public:
  using value_type = std::variant<ConditionalPtr, AssignExpPtr>;
  explicit Assignment(value_type v) : value_(std::move(v)) {}
  ~Assignment() = default;

  value_type &value() { return value_; }
 private:
  value_type value_;
};
using AssignmentPtr = std::shared_ptr<Assignment>;

class Expression {
 public:
  explicit Expression(AssignmentPtr assignment)
      : assignment_(std::move(assignment)) {}
  ~Expression() = default;
  AssignmentPtr &assignment() { return assignment_; }
 private:
  AssignmentPtr assignment_;
};

class ExpressionList {
 public:
  ExpressionList() = default;
  explicit ExpressionList(std::vector<ExpressionPtr> exps)
      : expression_vec_(std::move(exps)) {}
  ~ExpressionList() = default;

  std::vector<ExpressionPtr> &expression_vec() { return expression_vec_; }
 private:
  std::vector<ExpressionPtr> expression_vec_;
};

class Declaration {
 public:
  Declaration(VariablePtr var, ExpressionPtr init_exp)
      : var_(std::move(var)), init_exp_(std::move(init_exp)) {}
  ~Declaration() = default;
  VariablePtr &var() { return var_; }
  ExpressionPtr &init_exp() { return init_exp_; }
 private:
  VariablePtr var_;
  ExpressionPtr init_exp_;  // nullptr for not initialized
};
using DeclarationPtr = std::shared_ptr<Declaration>;

class Statement;
using StatementPtr = std::shared_ptr<Statement>;
class CompoundStatement;
using CompoundStatementPtr = std::shared_ptr<CompoundStatement>;

// 'return' expression ';'
class ReturnStatement {
 public:
  explicit ReturnStatement(ExpressionPtr exp) : exp_(std::move(exp)) {}
  ~ReturnStatement() = default;

  ExpressionPtr &exp() { return exp_; }
 private:
  ExpressionPtr exp_;
};
// expression? ';'
class ExpStatement {
 public:
  explicit ExpStatement(ExpressionPtr exp) : exp_(std::move(exp)) {}
  ~ExpStatement() = default;

  ExpressionPtr &exp() { return exp_; }
 private:
  ExpressionPtr exp_;
};
// 'if' '(' expression ')' statement ('else' statement)?
class IfStatement {
 public:
  IfStatement(ExpressionPtr cond_exp, StatementPtr if_stmt_, StatementPtr else_stmt_)
      : cond_exp_(std::move(cond_exp)), if_stmt_(std::move(if_stmt_)), else_stmt_(std::move(else_stmt_)) {}
  ~IfStatement() = default;
  ExpressionPtr &cond_exp() { return cond_exp_; }
  StatementPtr &if_stmt() { return if_stmt_; }
  StatementPtr &else_stmt() { return else_stmt_; }
 private:
  ExpressionPtr cond_exp_;
  StatementPtr if_stmt_;
  StatementPtr else_stmt_;
};
// 'for' '(' expression? ';' expression? ';' expression? ')' statement
class ForExpStatement {
 public:
  ForExpStatement(ExpressionPtr init_exp,
                  ExpressionPtr cond_exp,
                  ExpressionPtr update_exp,
                  StatementPtr statement)
      : init_exp_(std::move(init_exp)),
        cond_exp_(std::move(cond_exp)),
        update_exp_(std::move(update_exp)),
        statement_(std::move(statement)) {}
  ~ForExpStatement() = default;

  ExpressionPtr &init_exp() { return init_exp_; }
  ExpressionPtr &cond_exp() { return cond_exp_; }
  ExpressionPtr &update_exp() { return update_exp_; }
  StatementPtr &statement() { return statement_; }
 private:
  ExpressionPtr init_exp_;
  ExpressionPtr cond_exp_;
  ExpressionPtr update_exp_;
  StatementPtr statement_;
};
// 'for' '(' declaration expression? ';' expression? ')' statement
class ForDecStatement {
 public:
  ForDecStatement(DeclarationPtr init_decl,
                  ExpressionPtr cond_exp,
                  ExpressionPtr update_exp,
                  StatementPtr statement)
      : init_decl_(std::move(init_decl)),
        cond_exp_(std::move(cond_exp)),
        update_exp_(std::move(update_exp)),
        statement_(std::move(statement)) {}
  ~ForDecStatement() = default;

  DeclarationPtr &init_decl() { return init_decl_; }
  ExpressionPtr &cond_exp() { return cond_exp_; }
  ExpressionPtr &update_exp() { return update_exp_; }
  StatementPtr &statement() { return statement_; }
 private:
  DeclarationPtr init_decl_;
  ExpressionPtr cond_exp_;
  ExpressionPtr update_exp_;
  StatementPtr statement_;
};
// 'while' '(' expression ')' statement
class WhileStatement {
 public:
  WhileStatement(ExpressionPtr cond_exp, StatementPtr statement)
      : cond_exp_(std::move(cond_exp)), statement_(std::move(statement)) {}
  ~WhileStatement() = default;

  ExpressionPtr &cond_exp() { return cond_exp_; }
  StatementPtr &statement() { return statement_; }
 private:
  ExpressionPtr cond_exp_;
  StatementPtr statement_;
};
// 'do' statement 'while' '(' expression ')' ';'
class DoStatement {
 public:
  DoStatement(StatementPtr statement, ExpressionPtr cond_exp)
      : statement_(std::move(statement)), cond_exp_(std::move(cond_exp)) {}
  ~DoStatement() = default;

  StatementPtr &statement() { return statement_; }
  ExpressionPtr &cond_exp() { return cond_exp_; }
 private:
  StatementPtr statement_;
  ExpressionPtr cond_exp_;
};
// 'break' ';'
class BreakStatement {};
// 'continue' ';'
class ContinueStatement {};

using ReturnStatementPtr = std::shared_ptr<ReturnStatement>;
using ExpStatementPtr = std::shared_ptr<ExpStatement>;
using IfStatementPtr = std::shared_ptr<IfStatement>;
using ForExpStatementPtr = std::shared_ptr<ForExpStatement>;
using ForDecStatementPtr = std::shared_ptr<ForDecStatement>;
using WhileStatementPtr = std::shared_ptr<WhileStatement>;
using DoStatementPtr = std::shared_ptr<DoStatement>;
using BreakStatementPtr = std::shared_ptr<BreakStatement>;
using ContinueStatementPtr = std::shared_ptr<ContinueStatement>;

class Statement {
 public:
  using value_type = std::variant<ReturnStatementPtr,
                                  ExpStatementPtr,
                                  IfStatementPtr,
                                  CompoundStatementPtr,
                                  ForExpStatementPtr,
                                  ForDecStatementPtr,
                                  WhileStatementPtr,
                                  DoStatementPtr,
                                  BreakStatementPtr,
                                  ContinueStatementPtr>;
  explicit Statement(value_type p) : value_(std::move(p)) {}
  ~Statement() = default;

  value_type &value() { return value_; }
 private:
  value_type value_;
};

class BlockItem {
  using value_type = std::variant<StatementPtr, DeclarationPtr>;
 public:
  explicit BlockItem(const StatementPtr &stmt) : value_(stmt) {}
  explicit BlockItem(const DeclarationPtr &decl) : value_(decl) {}
  ~BlockItem() = default;

  value_type &value() { return value_; }
 private:
  value_type value_;
};
using BlockItemPtr = std::shared_ptr<BlockItem>;

class CompoundStatement {
 public:
  explicit CompoundStatement(std::vector<BlockItemPtr> vec) : block_item_vec_(std::move(vec)) {}
  ~CompoundStatement() = default;
  std::vector<BlockItemPtr> &block_item_vec() { return block_item_vec_; }
 private:
  std::vector<BlockItemPtr> block_item_vec_;
};
using CompoundStatementPtr = std::shared_ptr<CompoundStatement>;

class ParameterList {
 public:
  ParameterList() = default;
  explicit ParameterList(std::vector<VariablePtr> vars) : variable_vec_(std::move(vars)) {}
  ~ParameterList() = default;
  std::vector<VariablePtr> &variables() { return variable_vec_; }
 private:
  std::vector<VariablePtr> variable_vec_;
};
using ParameterListPtr = std::shared_ptr<ParameterList>;

class BaseType {
 public:
  BaseType() : type_(SupportType::Int) {}
  explicit BaseType(SupportType type) : type_(type) {}
  ~BaseType() = default;

  SupportType &type() { return type_; }
  void set_type(SupportType type) { type_ = type; }
 private:
  SupportType type_;
};

class Function {
 public:
  Function(BaseType ret_type,
           std::string func_name,
           ParameterListPtr parameter_list,
           CompoundStatementPtr compound_statement)
      : ret_type_(ret_type),
        func_name_(std::move(func_name)),
        parameter_list_(std::move(parameter_list)),
        compound_statement_(std::move(compound_statement)) {}
  ~Function() = default;

  std::string &name() { return func_name_; }
  BaseType &ret_type() { return ret_type_; }
  ParameterListPtr &parameter_list() { return parameter_list_; }
  CompoundStatementPtr &compound_statement() { return compound_statement_; }
 private:
  BaseType ret_type_;
  std::string func_name_;
  ParameterListPtr parameter_list_;
  CompoundStatementPtr compound_statement_; // nullptr for declaration
};
using FunctionPtr = std::shared_ptr<Function>;

class Program {
 public:
  using value_type = std::variant<FunctionPtr, DeclarationPtr>;
  explicit Program(std::vector<value_type> vec) : func_decl_vec_(std::move(vec)) {}
  ~Program() = default;

  std::vector<value_type> &func_decl_vec() { return func_decl_vec_; }
 private:
  std::vector<value_type> func_decl_vec_;
};
using ProgramPtr = std::shared_ptr<Program>;

#endif //SCOMPILER_SRC_BASE_AST_HPP_
