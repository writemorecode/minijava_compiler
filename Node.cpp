#include "Node.h"

#include <algorithm>

bool Node::buildTable(SymbolTable& st) const
{
    std::for_each(children.begin(), children.end(),
        [&](Node* child) { return child->buildTable(st); });
    return true;
}

std::string Node::checkTypes(SymbolTable& st) const
{
    for (const auto& child : children) {
        child->checkTypes(st);
    }
    return "";
}

void Node::print(int depth = 0) const
{
    for (int i = 0; i < depth; i++) {
        std::cout << "  ";
    }
    std::cerr << type << ":" << value << '\n';
    auto next_depth = depth + 1;
    for (const auto& child : children) {
        child->print(next_depth);
    }
}

void Node::printGraphviz(int& count, std::ostream& outStream)
{
    id = count++;
    outStream << "n" << id << " [label=\"" << type;
    if (value != "") {
        outStream << ": " << value;
    }
    outStream << "\"];" << '\n';

    for (const auto& child : children) {
        child->printGraphviz(count, outStream);
        outStream << "n" << id << " -> n" << child->id << '\n';
    }
}
