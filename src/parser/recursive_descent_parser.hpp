#ifndef SCOMPILER_SRC_PARSER_RECURSIVE_DESCENT_PARSER_HPP_
#define SCOMPILER_SRC_PARSER_RECURSIVE_DESCENT_PARSER_HPP_

#include "visitor.hpp"
#include "token_stream.hpp"

class RecursiveDescentParser : ASTVisitor {
 public:
  explicit RecursiveDescentParser(TokenStream &token_stream) : token_stream_(token_stream) {}
  ~RecursiveDescentParser() = default;
  ProgramPtr parse();
 private:
  void visit(ProgramPtr &program) override;
  void visit(FunctionPtr &function) override;
  void visit(BaseType &type) override;
  void visit(ParameterListPtr &parameter_list) override;
  void visit(CompoundStatementPtr &compound_statement) override;
  void visit(BlockItemPtr &block_item) override;
  void visit(StatementPtr &statement) override;
  void visit(DeclarationPtr &declaration) override;
  void visit(ExpressionListPtr &expression_list) override;
  void visit(ExpressionPtr &expression) override;
  void visit(AssignmentPtr &assignment) override;
  void visit(ConditionalPtr &conditional) override;
  void visit(LogicalOrPtr &logical_or) override;
  void visit(LogicalAndPtr &logical_and) override;
  void visit(EqualityPtr &equality) override;
  void visit(RelationalPtr &relational) override;
  void visit(AdditivePtr &additive) override;
  void visit(MultiplicativePtr &multiplicative) override;
  void visit(UnaryPtr &unary) override;
  void visit(PostfixPtr &postfix) override;
  void visit(PrimaryPtr &primary) override;

  TokenStream &token_stream_;
};

#endif //SCOMPILER_SRC_PARSER_RECURSIVE_DESCENT_PARSER_HPP_
