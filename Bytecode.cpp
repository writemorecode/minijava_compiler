#include "Bytecode.hpp"

#include <iostream>

BytecodeMethod &BytecodeProgram::addMethod(const std::string &name) {
    const auto &[it, failed] = methods.emplace(name);
    if (failed) {
        std::cerr << "Error BytecodeProgram::addMethod: Failed to insert '"
                  << name << "' into map.\n'";
    }
    return it->second;
}

BytecodeMethodBlock &BytecodeMethod::addMethodBlock(const std::string &name) {
    return blocks.emplace_back(name);
}
