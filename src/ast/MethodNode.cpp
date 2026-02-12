#include "ast/MethodNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }
