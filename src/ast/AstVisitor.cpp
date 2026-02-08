#include "ast/AstVisitor.hpp"

#include "ast/ClassNode.hpp"
#include "ast/MainClassNode.hpp"
#include "ast/MethodNode.hpp"
#include "ast/MethodParameterNode.hpp"
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/Node.h"
#include "ast/VariableNode.hpp"

void AstVisitor::visit(const Node &node) {
    for (const auto &child : node.children) {
        child->accept(*this);
    }
}

void AstVisitor::visit(const ClassNode &node) {
    visit(static_cast<const Node &>(node));
}

void AstVisitor::visit(const MainClassNode &node) {
    visit(static_cast<const Node &>(node));
}

void AstVisitor::visit(const MethodNode &node) {
    visit(static_cast<const Node &>(node));
}

void AstVisitor::visit(const MethodWithoutParametersNode &node) {
    visit(static_cast<const Node &>(node));
}

void AstVisitor::visit(const MethodParameterNode &node) {
    visit(static_cast<const Node &>(node));
}

void AstVisitor::visit(const VariableNode &node) {
    visit(static_cast<const Node &>(node));
}
