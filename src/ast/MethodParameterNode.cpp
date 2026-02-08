#include "ast/MethodParameterNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodParameterNode::accept(AstVisitor &visitor) const {
    visitor.visit(*this);
}
