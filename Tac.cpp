#include "Tac.hpp"
#include <iostream>

void Tac::print() const {
    std::cout << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void ExpressionTac::print() const {
    std::cout << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void UnaryExpressionTac::print() const {
    std::cout << result << " := " << op << " " << rhs << "\n";
}

void CopyTac::print() const { std::cout << result << " := " << lhs << "\n"; }

void ArrayCopyTac::print() const {
    std::cout << result << "[" << lhs << "]"
              << " := " << rhs << "\n";
}

void ArrayAccessTac::print() const {
    std::cout << result << " := " << lhs << "[" << rhs << "]\n";
}
