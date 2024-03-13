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

void NewTac::print() const { std::cout << result << " := new " << rhs << "\n"; }

void NewArrayTac::print() const {
    std::cout << result << " := new int, " << rhs << "\n";
}

void JumpTac::print() const { std::cout << op << " " << result << "\n"; }

void CondJumpTac::print() const {
    std::cout << "iffalse " << lhs << " goto " << rhs << "\n";
}
