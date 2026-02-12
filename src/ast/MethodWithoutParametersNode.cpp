#include "ast/MethodWithoutParametersNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodWithoutParametersNode::accept(AstVisitor &visitor) const {
    visitor.visit(*this);
}
