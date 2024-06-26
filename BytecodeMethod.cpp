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
    const auto &it = std::find(blocks.begin(), blocks.end(), name);
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

void BytecodeMethod::serialize(Serializer &serializer) const {
    serializer.writeString(name);
    serializer.writeStringVector(variables);
    serializer.writeInteger(blocks.size());
    for (const auto &block : blocks) {
        serializer.writeString(block.getName());
        block.serialize(serializer);
    }
}
