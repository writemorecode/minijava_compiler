#include "ast/Node.h"

#include "ast/AstVisitor.hpp"
#include "semantic/SymbolTable.hpp"
#include "semantic/SymbolTableVisitor.hpp"

bool Node::buildTable(SymbolTable &st) const {
    return build_symbol_table(*this, st).ok();
}

std::string Node::checkTypes(SymbolTable &st) const {
    bool valid = true;
    for (const auto &child : children) {
        auto type = child->checkTypes(st);
        if (type.empty()) {
            valid = false;
        }
    }
    if (valid) {
        return "void";
    }
    return "";
}

Operand Node::generateIR(CFG &graph, SymbolTable &st) {
    for (auto &child : children) {
        child->generateIR(graph, st);
    }
    return "foobar";
}

void Node::accept(AstVisitor &visitor) const { visitor.visit(*this); }

void Node::print(int depth = 0) const {
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cerr << type << ":" << value << '\n';
    auto next_depth = depth + 1;
    for (const auto &child : children) {
        child->print(next_depth);
    }
}

void Node::printGraphviz(int &count, std::ostream &outStream) {
    id = count++;
    outStream << "n" << id << " [label=\"" << type;
    if (!value.empty()) {
        outStream << ": " << value;
    }
    outStream << "\"];" << '\n';

    for (const auto &child : children) {
        child->printGraphviz(count, outStream);
        outStream << "n" << id << " -> n" << child->id << '\n';
    }
}
