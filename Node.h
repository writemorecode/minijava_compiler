#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "BBlock.hpp"
#include "CFG.hpp"
#include "SymbolTable.hpp"

class Node {
  public:
    std::string type{}, value{};
    int id = 0, lineno = 0;
    std::list<Node *> children;

    Node(const std::string &t, int l) : type(t), lineno(l) {}
    Node(const std::string &t, const std::string &v, int l)
        : type(t), value(v), lineno(l) {}
    Node(const std::string &t, int l, std::list<Node *> children_)
        : type(t), lineno(l), children(std::move(children_)) {}
    virtual ~Node() = default;

    virtual bool buildTable(SymbolTable &st) const;

    virtual std::string checkTypes(SymbolTable &st) const;

    virtual std::string generateIR(CFG &graph);

    void print(int depth) const;
    void printGraphviz(int &count, std::ostream &outStream);
};

#endif
