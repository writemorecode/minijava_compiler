#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

class Node;
class ClassNode;
class MainClassNode;
class MethodNode;
class MethodWithoutParametersNode;
class MethodParameterNode;
class VariableNode;

class AstVisitor {
  public:
    virtual ~AstVisitor() = default;

    virtual void visit(const Node &node);
    virtual void visit(const ClassNode &node);
    virtual void visit(const MainClassNode &node);
    virtual void visit(const MethodNode &node);
    virtual void visit(const MethodWithoutParametersNode &node);
    virtual void visit(const MethodParameterNode &node);
    virtual void visit(const VariableNode &node);
};

#endif
