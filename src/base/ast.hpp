#ifndef SCOMPILER_SRC_BASE_AST_HPP_
#define SCOMPILER_SRC_BASE_AST_HPP_

#include "token.hpp"
#include "variable.hpp"

#include <utility>
#include <vector>
#include <memory>

class Expression;
using ExpressionPtr = std::shared_ptr<Expression>;

class Primary {
 public:
  using value_type = std::variant<int, ExpressionPtr, std::string>;
  enum class Type {
    Integer,
    Expression,
    Identifier,
  };
  explicit Primary(int v) : type_(Type::Integer), value_(v) {}
  explicit Primary(const ExpressionPtr &exp) : type_(Type::Expression), value_(exp) {}
  explicit Primary(const std::string &id) : type_(Type::Identifier), value_(id) {}
  ~Primary() = default;
 private:
  Type type_;
  value_type value_;
};
using PrimaryPtr = std::shared_ptr<Primary>;

class ExpressionList;
using ExpressionListPtr = std::shared_ptr<ExpressionList>;

class Postfix {
  class FuncCall {
   public:
    FuncCall(std::string &func_name, ExpressionListPtr exp_list)
        : func_name_(func_name), expression_list_(std::move(exp_list)) {}
    ~FuncCall() = default;
   private:
    std::string func_name_;
    ExpressionListPtr expression_list_;
  };
  class Array {
   public:
    Array(int dimension, std::vector<ExpressionPtr> expressions)
        : dimension_(dimension), expressions_(std::move(expressions)) {}
    ~Array() = default;
   private:
    int dimension_;
    std::vector<ExpressionPtr> expressions_;
  };
  using FuncCallPtr = std::shared_ptr<FuncCall>;
  using ArrayPtr = std::shared_ptr<Array>;
 public:
  using value_type = std::variant<PrimaryPtr, FuncCallPtr, ArrayPtr>;
  enum class Type {
    Primary,
    FuncCall,
    Array,
  };
  explicit Postfix(const PrimaryPtr &p) : type_(Type::Primary), value_(p) {}
  Postfix(std::string &fname, const ExpressionListPtr &exp_list)
      : type_(Type::FuncCall),
        value_(std::make_shared<FuncCall>(FuncCall(fname, exp_list))) {}
  Postfix(int d, const std::vector<ExpressionPtr> &exps)
      : type_(Type::FuncCall),
        value_(std::make_shared<Array>(d, exps)) {}
  ~Postfix() = default;
 private:
  Type type_;
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
  Unary(UnaryPtr left, Op op, value_type right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Unary() = default;
 private:
  UnaryPtr left_;
  Op op_;
  std::variant<PostfixPtr, UnaryPtr> right_;
};
using UnaryPtr = std::shared_ptr<Unary>;

class Multiplicative {
  using MultiplicativePtr = std::shared_ptr<Multiplicative>;
 public:
  using value_type = std::variant<MultiplicativePtr, UnaryPtr>;
  enum class Op {
    Times,
    Divide,
    Mod,
  };
  Multiplicative(MultiplicativePtr left, Op op, value_type right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Multiplicative() = default;
 private:
  MultiplicativePtr left_;
  Op op_;
  std::variant<std::shared_ptr<Multiplicative>, UnaryPtr> right_;
};
using MultiplicativePtr = std::shared_ptr<Multiplicative>;

class Additive {
  using AdditivePtr = std::shared_ptr<Additive>;
 public:
  using value_type = std::variant<AdditivePtr, MultiplicativePtr>;
  enum class Op {
    Add,
    Sub,
  };
  Additive(AdditivePtr left, Op op, value_type right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Additive() = default;
 private:
  AdditivePtr left_;
  Op op_;
  value_type right_;
};
using AdditivePtr = std::shared_ptr<Additive>;

class Relational {
  using RelationalPtr = std::shared_ptr<Relational>;
 public:
  using value_type = std::variant<RelationalPtr, AdditivePtr>;
  enum class Op {
    Less,
    Greater,
    Le,
    Ge,
  };
  Relational(RelationalPtr left, Op op, value_type right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Relational() = default;
 private:
  RelationalPtr left_;
  Op op_;
  value_type right_;
};
using RelationalPtr = std::shared_ptr<Relational>;

class Equality {
  using EqualityPtr = std::shared_ptr<Equality>;
 public:
  using value_type = std::variant<EqualityPtr, RelationalPtr>;
  enum class Op {
    Equal,
    Nequal,
  };
  Equality(EqualityPtr left, Op op, value_type right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  ~Equality() = default;
 private:
  EqualityPtr left_;
  Op op_;
  value_type right_;
};
using EqualityPtr = std::shared_ptr<Equality>;

class LogicalAnd {
  using LogicalAndPtr = std::shared_ptr<LogicalAnd>;
 public:
  using value_type = std::variant<LogicalAndPtr, EqualityPtr>;
  LogicalAnd(LogicalAndPtr left, value_type right)
      : left_(std::move(left)), right_(std::move(right)) {}
  ~LogicalAnd() = default;
 private:
  LogicalAndPtr left_;
  value_type right_;
};
using LogicalAndPtr = std::shared_ptr<LogicalAnd>;

class LogicalOr {
  using LogicalOrPtr = std::shared_ptr<LogicalOr>;
  using value_type = std::variant<LogicalOrPtr, LogicalAndPtr>;
 public:
  LogicalOr(LogicalOrPtr left, value_type right)
      : left_(std::move(left)), right_(std::move(right)) {}
  ~LogicalOr() = default;
 private:
  LogicalOrPtr left_;
  value_type right_;
};
using LogicalOrPtr = std::shared_ptr<LogicalOr>;

class Conditional {
  using ConditionalPtr = std::shared_ptr<Conditional>;
 public:
  Conditional(LogicalAndPtr cond, ExpressionPtr cond_true, ConditionalPtr cond_false)
      : cond_(std::move(cond)), cond_true_(std::move(cond_true)), cond_false_(std::move(cond_false)) {}
  ~Conditional() = default;
 private:
  LogicalAndPtr cond_;
  ExpressionPtr cond_true_;
  ConditionalPtr cond_false_;
};
using ConditionalPtr = std::shared_ptr<Conditional>;

class Assignment {
  class AssignExp {
   public:
    AssignExp(UnaryPtr left, ExpressionPtr right)
        : left_(std::move(left)), right_(std::move(right)) {}
    ~AssignExp() = default;
   private:
    UnaryPtr left_;
    ExpressionPtr right_;
  };
  using AssignExpPtr = std::shared_ptr<AssignExp>;
 public:
  using value_type = std::variant<ConditionalPtr, AssignExpPtr>;
  enum class Type {
    Cond,
    Assign,
  };
  explicit Assignment(const ConditionalPtr &v)
      : type_(Type::Cond), value_(v) {}
  Assignment(const UnaryPtr &left, const ExpressionPtr &right)
      : type_(Type::Assign), value_(std::make_shared<AssignExp>(left, right)) {}
  ~Assignment() = default;
 private:
  Type type_;
  value_type value_;
};
using AssignmentPtr = std::shared_ptr<Assignment>;

class Expression {
 public:
  explicit Expression(AssignmentPtr assignment)
      : assignment_(std::move(assignment)) {}
  ~Expression() = default;
 private:
  AssignmentPtr assignment_;
};

class ExpressionList {
 public:
  explicit ExpressionList(std::vector<ExpressionPtr> exps)
      : expression_vec_(std::move(exps)) {}
  ~ExpressionList() = default;
 private:
  std::vector<ExpressionPtr> expression_vec_;
};

class Declaration {
 public:
  Declaration(const Variable &var, ExpressionPtr init_exp)
      : var_(var), init_exp_(std::move(init_exp)) {}
  ~Declaration() = default;
 private:
  Variable var_;
  ExpressionPtr init_exp_;
};
using DeclarationPtr = std::shared_ptr<Declaration>;

class Statement {
  using StatementPtr = std::shared_ptr<Statement>;
  class ReturnStatement {
   public:
    explicit ReturnStatement(ExpressionPtr exp) : exp_(std::move(exp)) {}
    ~ReturnStatement() = default;
   private:
    ExpressionPtr exp_;
  };
  class ExpStatement {
   public:
    explicit ExpStatement(ExpressionPtr exp) : exp_(std::move(exp)) {}
    ~ExpStatement() = default;
   private:
    ExpressionPtr exp_;
  };
  class IfStatement {
   public:
    IfStatement(ExpressionPtr cond_exp, StatementPtr if_stmt_, StatementPtr else_stmt_)
        : cond_exp_(std::move(cond_exp)), if_stmt_(std::move(if_stmt_)), else_stmt_(std::move(else_stmt_)) {}
    ~IfStatement() = default;
   private:
    ExpressionPtr cond_exp_;
    StatementPtr if_stmt_;
    StatementPtr else_stmt_;
  };
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
   private:
    ExpressionPtr init_exp_;
    ExpressionPtr cond_exp_;
    ExpressionPtr update_exp_;
    StatementPtr statement_;
  };
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
   private:
    DeclarationPtr init_decl_;
    ExpressionPtr cond_exp_;
    ExpressionPtr update_exp_;
    StatementPtr statement_;
  };
  class WhileStatement {
   public:
    WhileStatement(ExpressionPtr cond_exp, StatementPtr statement)
        : cond_exp_(std::move(cond_exp)), statement_(std::move(statement)) {}
    ~WhileStatement() = default;
   private:
    ExpressionPtr cond_exp_;
    StatementPtr statement_;
  };
  class DoStatement {
   public:
    DoStatement(StatementPtr statement, ExpressionPtr cond_exp)
        : statement_(std::move(statement)), cond_exp_(std::move(cond_exp)) {}
    ~DoStatement() = default;
   private:
    StatementPtr statement_;
    ExpressionPtr cond_exp_;
  };
  using ReturnStatementPtr = std::shared_ptr<ReturnStatement>;
  using ExpStatementPtr = std::shared_ptr<ExpStatement>;
  using IfStatementPtr = std::shared_ptr<IfStatement>;
  using ForExpStatementPtr = std::shared_ptr<ForExpStatement>;
  using ForDecStatementPtr = std::shared_ptr<ForDecStatement>;
  using WhileStatementPtr = std::shared_ptr<WhileStatement>;
  using DoStatementPtr = std::shared_ptr<DoStatement>;
 public:
  using value_type = std::variant<ReturnStatementPtr,
                                  ExpStatementPtr,
                                  IfStatementPtr,
                                  ForExpStatementPtr,
                                  ForDecStatementPtr,
                                  WhileStatementPtr,
                                  DoStatementPtr>;
  enum class Type {
    Return,
    Exp,
    If,
    ForExp,
    ForDec,
    While,
    Do,
    Break,
    Continue,
  };
  Statement(Type type, const ExpressionPtr &exp) : type_(Type::Return), value_(std::make_shared<ReturnStatement>(exp)) {
    assert(type == Type::Return);
  }
  explicit Statement(const ExpressionPtr &exp) : type_(Type::Exp), value_(std::make_shared<ExpStatement>(exp)) {}
  Statement(const ExpressionPtr &cond_exp,
            const StatementPtr &if_stmt,
            const StatementPtr &else_stmt)
      : type_(Type::If), value_(std::make_shared<IfStatement>(cond_exp, if_stmt, else_stmt)) {}
  Statement(const ExpressionPtr &init_exp,
            const ExpressionPtr &cond_exp,
            const ExpressionPtr &update_exp,
            const StatementPtr &stmt)
      : type_(Type::ForExp), value_(std::make_shared<ForExpStatement>(init_exp, cond_exp, update_exp, stmt)) {}
  Statement(const DeclarationPtr &init_decl,
            const ExpressionPtr &cond_exp,
            const ExpressionPtr &update_exp,
            const StatementPtr &stmt)
      : type_(Type::ForDec), value_(std::make_shared<ForDecStatement>(init_decl, cond_exp, update_exp, stmt)) {}
  Statement(const ExpressionPtr &cond_exp,
            const StatementPtr &stmt)
      : type_(Type::While), value_(std::make_shared<WhileStatement>(cond_exp, stmt)) {}
  Statement(const StatementPtr &stmt,
            const ExpressionPtr &cond_exp)
      : type_(Type::Do), value_(std::make_shared<DoStatement>(stmt, cond_exp)) {}
  explicit Statement(Type type) : type_(type) {
    assert(type == Type::Break || type == Type::Continue);
  }
  ~Statement() = default;
 private:
  Type type_;
  value_type value_;
};
using StatementPtr = std::shared_ptr<Statement>;

class BlockItem {
  using value_type = std::variant<StatementPtr, DeclarationPtr>;
 public:
  enum class Type {
    Stmt,
    Decl,
  };
  explicit BlockItem(const StatementPtr &stmt) : type_(Type::Stmt), value_(stmt) {}
  explicit BlockItem(const DeclarationPtr &decl) : type_(Type::Stmt), value_(decl) {}
  ~BlockItem() = default;
 private:
  Type type_;
  value_type value_;
};
using BlockItemPtr = std::shared_ptr<BlockItem>;

class CompoundStatement {
 public:
  explicit CompoundStatement(std::vector<BlockItemPtr> vec) : block_item_vec_(std::move(vec)) {}
  ~CompoundStatement() = default;
 private:
  std::vector<BlockItemPtr> block_item_vec_;
};
using CompoundStatementPtr = std::shared_ptr<CompoundStatement>;

class ParameterList {
 public:
  explicit ParameterList(std::vector<Variable> vars) : variables_(std::move(vars)) {}
  ~ParameterList() = default;
 private:
  std::vector<Variable> variables_;
};
using ParameterListPtr = std::shared_ptr<ParameterList>;

class BaseType {
 public:
  enum Type {
    Int,
  };
  explicit BaseType(Type type) : type_(type) {}
  ~BaseType() = default;
 private:
  Type type_;
};

class Function {
 public:
  Function(BaseType ret_type, ParameterListPtr parameter_list, CompoundStatementPtr compound_statement)
      : ret_type_(ret_type),
        parameter_list_(std::move(parameter_list)),
        compound_statement_(std::move(compound_statement)) {}
  ~Function() = default;
 private:
  BaseType ret_type_;
  ParameterListPtr parameter_list_;
  CompoundStatementPtr compound_statement_;
};
using FunctionPtr = std::shared_ptr<Function>;

class Program {
 public:
  Program(std::vector<FunctionPtr> functions, std::vector<DeclarationPtr> declarations)
      : functions_(std::move(functions)), declarations_(std::move(declarations)) {}
  ~Program() = default;
 private:
  std::vector<FunctionPtr> functions_;
  std::vector<DeclarationPtr> declarations_;
};
using ProgramPtr = std::shared_ptr<Program>;

#endif //SCOMPILER_SRC_BASE_AST_HPP_
