#include "ast/MainClassNode.hpp"

#include "ast/AstVisitor.hpp"

void MainClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }
