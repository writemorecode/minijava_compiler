#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "ir/CFG.hpp"

class Node {
  public:
    std::string type{}, value{};
    int id = 0, lineno = 0;
    std::list<std::unique_ptr<Node>> children;

    Node(const std::string &t, int l) : type(t), lineno(l) {}
    Node(const std::string &t, const std::string &v, int l)
        : type(t), value(v), lineno(l) {}
    Node(const std::string &t, int l, std::list<std::unique_ptr<Node>> children_)
        : type(t), lineno(l), children(std::move(children_)) {}
    virtual ~Node() = default;

    virtual bool buildTable(SymbolTable &st) const;

    virtual std::string checkTypes(SymbolTable &st) const;

    virtual Operand generateIR(CFG &graph, SymbolTable &st);

    void print(int depth) const;
    void printGraphviz(int &count, std::ostream &outStream);

    Node(const Node &other) = delete;
    const Node &operator=(const Node &other) = delete;

  protected:
    Node *append_child(std::unique_ptr<Node> child) {
        Node *raw = child.get();
        children.push_back(std::move(child));
        return raw;
    }
};

#endif
