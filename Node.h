#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "SymbolTable.hpp"

class Node {
  public:
    std::string type{"uninitialized"}, value{"uninitialized"};
    int id = 0, lineno = 0;
    std::list<Node *> children;

    Node(std::string t, int l) : type(std::move(t)), value(""), lineno(l) {}
    Node(std::string t, std::string v, int l)
        : type(std::move(t)), value(std::move(v)), lineno(l) {}
    Node(std::string t, int l, std::list<Node *> children_)
        : type(std::move(t)), value(""), lineno(l),
          children(std::move(children_)) {}
    virtual ~Node() = default;

    virtual bool buildTable(SymbolTable &st) const;

    virtual std::string checkTypes(SymbolTable &st) const;

    void print(int depth) const;
    void printGraphviz(int &count, std::ostream &outStream);
};

#endif
