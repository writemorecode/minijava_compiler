#include "ast/VariableNode.hpp"

#include "ast/AstVisitor.hpp"

void VariableNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }
