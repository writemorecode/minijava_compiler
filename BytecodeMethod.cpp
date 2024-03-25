#include "BytecodeMethod.hpp"

BytecodeMethodBlock &BytecodeMethod::addMethodBlock(const std::string &name) {
    return blocks.emplace_back(name);
}

void BytecodeMethod::print(std::ostream &os) const {
    for (const auto &block : blocks) {
        block.print(os);
    }
}

BytecodeMethodBlock &BytecodeMethod::getFirstBlock() { return blocks.front(); }
