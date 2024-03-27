#include "BytecodeMethod.hpp"
#include "BytecodeMethodBlock.hpp"
#include <algorithm>
#include <iostream>

BytecodeMethodBlock &
BytecodeMethod::addBytecodeMethodBlock(const std::string &name) {
    return blocks.emplace_back(name);
}
[[nodiscard]] BytecodeMethodBlock &
BytecodeMethod::getBytecodeMethodBlock(const std::string &name) {
    auto it = std::find_if(blocks.begin(), blocks.end(),
                           [&name](const BytecodeMethodBlock &block) {
                               return block.getName() == name;
                           });
    if (it == blocks.end()) {
        return addBytecodeMethodBlock(name);
    }
    return *it;
}

void BytecodeMethod::print(std::ostream &os) const {
    for (const auto &block : blocks) {
        block.print(os);
    }
}

BytecodeMethodBlock &BytecodeMethod::getFirstBlock() { return blocks.front(); }
