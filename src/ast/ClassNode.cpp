#include "ast/ClassNode.hpp"

#include "ast/AstVisitor.hpp"

void ClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }
