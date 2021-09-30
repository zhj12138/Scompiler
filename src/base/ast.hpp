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
  Primary(Type type, value_type value) : type_(type), value_(std::move(value)) {}
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
 private:
  std::string func_name_;
  ExpressionListPtr expression_list_;
};
using FuncCallPtr = std::shared_ptr<FuncCall>;

class Array {
 public:
 private:
  int dimension_;
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

 private:
  Type type_;
  value_type value_;
};
using PostfixPtr = std::shared_ptr<Postfix>;

class Unary {
 public:
  enum class Op {
    Sub,
    Not,
    Lnot,
  };
 private:
  std::shared_ptr<Unary> left_;
  Op op_;
  std::variant<PostfixPtr, std::shared_ptr<Unary>> right_;
};
using UnaryPtr = std::shared_ptr<Unary>;

class Multiplicative {
 public:
  enum class Op {
    Times,
    Divide,
    Mod,
  };
 private:
  std::shared_ptr<Multiplicative> left_;
  Op op_;
  std::variant<std::shared_ptr<Multiplicative>, UnaryPtr> right_;
};
using MultiplicativePtr = std::shared_ptr<Multiplicative>;

class Additive {
 public:
  enum class Op {
    Add,
    Sub,
  };
 private:
  std::shared_ptr<Additive> left_;
  Op op_;
  std::variant<std::shared_ptr<Additive>, MultiplicativePtr> right_;
};
using AdditivePtr = std::shared_ptr<Additive>;

class Relational {
 public:
  enum class Op {
    Less,
    Greater,
    Le,
    Ge,
  };
 private:
  std::shared_ptr<Relational> left_;
  Op op_;
  std::variant<std::shared_ptr<Relational>, Additive> right_;
};
using RelationalPtr = std::shared_ptr<Relational>;

class Equality {
 public:
  enum class Op {
    Equal,
    Nequal,
  };
 private:
  std::shared_ptr<Equality> left_;
  Op op_;
  std::variant<std::shared_ptr<Equality>, Relational> right_;
};
using EqualityPtr = std::shared_ptr<Equality>;

class LogicalAnd {
 public:
 private:
  std::shared_ptr<LogicalAnd> left_;
  std::variant<std::shared_ptr<LogicalAnd>, Equality> right_;
};
using LogicalAndPtr = std::shared_ptr<LogicalAnd>;

class LogicalOr {
 public:
 private:
  std::shared_ptr<LogicalOr> left_;
  std::variant<std::shared_ptr<LogicalOr>, LogicalAnd> right_;
};
using LogicalOrPtr = std::shared_ptr<LogicalOr>;

class Conditional {
 public:
 private:
  LogicalAndPtr cond_;
  ExpressionPtr cond_true_;
  std::shared_ptr<Conditional> cond_false_;
};
using ConditionalPtr = std::shared_ptr<Conditional>;

class Assignment {
  class AssignExp {
   public:
   private:
    UnaryPtr left_;
    ExpressionPtr right_;
  };
  using AssignExpPtr = std::shared_ptr<AssignExp>;
  using value_type = std::variant<ConditionalPtr, AssignExpPtr>;
 public:
  enum class Type {
    Condtion,
    Assign,
  };
 private:
  Type type_;
  value_type value_;
};
using AssignmentPtr = std::shared_ptr<Assignment>;

class Expression {
 public:
 private:
  AssignmentPtr assignment_;
};

class ExpressionList {
 public:
 private:
  std::vector<ExpressionPtr> expression_vec_;
};

class Declaration {
 public:
 private:
  Variable var_;
  ExpressionPtr init_exp_;
};
using DeclarationPtr = std::shared_ptr<Declaration>;

class Statement {
  using StatementPtr = std::shared_ptr<Statement>;
  class ReturnStatement {
   public:
   private:
    ExpressionPtr exp_;
  };
  class ExpStatement {
   public:
   private:
    ExpressionPtr exp_;
  };
  class IfStatement {
   public:
   private:
    ExpressionPtr cond_exp_;
    StatementPtr if_stmt_;
    StatementPtr else_stmt_;
  };
  class ForExpStatement {
   public:
   private:
    ExpressionPtr init_exp_;
    ExpressionPtr cond_exp_;
    ExpressionPtr update_exp_;
    StatementPtr statement_;
  };
  class ForDecStatement {
   public:
   private:
    DeclarationPtr init_decl_;
    ExpressionPtr cond_exp_;
    ExpressionPtr update_exp_;
    StatementPtr statement_;
  };
  class WhileStatement {
   public:
   private:
    ExpressionPtr cond_exp_;
    StatementPtr statement_;
  };
  class DoStatement {
   public:
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
  using value_type = std::variant<ReturnStatementPtr,
                                  ExpStatementPtr,
                                  IfStatementPtr,
                                  ForExpStatementPtr,
                                  ForDecStatementPtr,
                                  WhileStatementPtr,
                                  DoStatementPtr>;
 public:
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
 private:
  Type type_;
  value_type value_;
};
using BlockItemPtr = std::shared_ptr<BlockItem>;

class CompoundStatement {
 public:
 private:
  std::vector<BlockItemPtr> block_item_vec_;
};
using CompoundStatementPtr = std::shared_ptr<CompoundStatement>;

class ParameterList {
 public:
 private:
  std::vector<Variable> variables_;
};
using ParameterListPtr = std::shared_ptr<ParameterList>;

class BaseType {
 public:
 private:
};

using DeclarationPtr = std::shared_ptr<Declaration>;

class Function {
 public:
 private:
  BaseType ret_type_;
  ParameterListPtr parameter_list_;
  CompoundStatementPtr compound_statement_;
};
using FunctionPtr = std::shared_ptr<Function>;

class Program {
 public:
  Program() = default;
  ~Program() = default;
 private:
  std::vector<FunctionPtr> functions_;
  std::vector<DeclarationPtr> declarations_;
};
using ProgramPtr = std::shared_ptr<Program>;

#endif //SCOMPILER_SRC_BASE_AST_HPP_
