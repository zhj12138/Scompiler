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

class FuncCall {
 public:
  FuncCall(std::string &func_name, ExpressionListPtr exp_list)
      : func_name_(func_name), expression_list_(std::move(exp_list)) {}
  ~FuncCall() = default;
 private:
  std::string func_name_;
  ExpressionListPtr expression_list_;
};
using FuncCallPtr = std::shared_ptr<FuncCall>;
// 因为当前不支持指针，所以其实Array解析的时候postfix只能递归到primary,而不能递归到func call，不过为了符合文法，还是支持该语法
class Array {
  using nameType = std::variant<FuncCallPtr, PrimaryPtr>;
  enum class Type {
    PrimaryName,
    FuncCallName,
  };
 public:
  Array(FuncCallPtr name, std::vector<ExpressionPtr> expressions)
      : type_(Type::FuncCallName),
        name_(name),
        expressions_(std::move(expressions)) {}
  Array(PrimaryPtr name, std::vector<ExpressionPtr> expressions)
      : type_(Type::PrimaryName),
        name_(name),
        expressions_(std::move(expressions)) {}
  ~Array() = default;
 private:
  Type type_;
  nameType name_;
  std::vector<ExpressionPtr> expressions_;
};
using ArrayPtr = std::shared_ptr<Array>;

class Postfix {
 public:
  using value_type = std::variant<PrimaryPtr, FuncCallPtr, ArrayPtr>;
  enum class Type {
    Primary,
    FuncCall,
    Array,
  };
  explicit Postfix(const PrimaryPtr &p) : type_(Type::Primary), value_(p) {}
  explicit Postfix(const FuncCallPtr &p)
      : type_(Type::FuncCall), value_(p) {}
  Postfix(const PrimaryPtr &name, const std::vector<ExpressionPtr> &exps)
      : type_(Type::FuncCall),
        value_(std::make_shared<Array>(name, exps)) {}
  Postfix(const FuncCallPtr &name, const std::vector<ExpressionPtr> &exps)
      : type_(Type::FuncCall),
        value_(std::make_shared<Array>(name, exps)) {}
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
  explicit Unary(PostfixPtr value) : op_(Op::Sub), value_(std::move(value)) {}
  Unary(Op op, UnaryPtr value)
      : op_(op), value_(std::move(value)) {}
  ~Unary() = default;
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
 private:
  LogicalOrPtr left_;
  LogicalAndPtr right_;
};
using LogicalOrPtr = std::shared_ptr<LogicalOr>;

class Conditional {
  using ConditionalPtr = std::shared_ptr<Conditional>;
 public:
  Conditional(LogicalOrPtr cond, ExpressionPtr cond_true, ConditionalPtr cond_false)
      : cond_(std::move(cond)), cond_true_(std::move(cond_true)), cond_false_(std::move(cond_false)) {}
  ~Conditional() = default;
 private:
  LogicalOrPtr cond_;
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
  ExpressionList() = default;
  explicit ExpressionList(std::vector<ExpressionPtr> exps)
      : expression_vec_(std::move(exps)) {}
  ~ExpressionList() = default;
 private:
  std::vector<ExpressionPtr> expression_vec_;
};

class Declaration {
 public:
  Declaration(VariablePtr var, ExpressionPtr init_exp)
      : var_(std::move(var)), init_exp_(std::move(init_exp)) {}
  ~Declaration() = default;
 private:
  VariablePtr var_;
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
  ParameterList() = default;
  explicit ParameterList(std::vector<VariablePtr> vars) : variables_(std::move(vars)) {}
  ~ParameterList() = default;
 private:
  std::vector<VariablePtr> variables_;
};
using ParameterListPtr = std::shared_ptr<ParameterList>;

class BaseType {
 public:
  BaseType() : type_(SupportType::Int) {}
  explicit BaseType(SupportType type) : type_(type) {}
  ~BaseType() = default;

  SupportType type() { return type_; }
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
 private:
  BaseType ret_type_;
  std::string func_name_;
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
