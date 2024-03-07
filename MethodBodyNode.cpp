#include "MethodBodyNode.hpp"

std::string MethodBodyNode::checkTypes(SymbolTable& st) const
{
    body->checkTypes(st);
    return returnValue->checkTypes(st);
}
std::string ReturnOnlyMethodBodyNode::checkTypes(SymbolTable& st) const
{
    return returnValue->checkTypes(st);
}
