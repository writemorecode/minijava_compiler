#ifndef ARRAYACCESSNODE_HPP
#define ARRAYACCESSNODE_HPP

#include "ast/Node.h"

class ArrayAccessNode : public Node {
    Node *array, *index;

  public:
    ArrayAccessNode(std::unique_ptr<Node> array_, std::unique_ptr<Node> index_,
                    int l)
        : Node("Array access", l) {
        array = append_child(std::move(array_));
        index = append_child(std::move(index_));
    }
};

#endif // ARRAYACCESSNODE_HPP
