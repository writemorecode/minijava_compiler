#include "BytecodeProgram.hpp"

#include <iostream>

BytecodeMethod &BytecodeProgram::addBytecodeMethod(const std::string &name) {
    const auto &it = methods.emplace(name, name);
    return it.first->second;
}

BytecodeMethod &BytecodeProgram::getBytecodeMethod(const std::string &name) {
    const auto &it = methods.find(name);
    if (it == methods.end()) {
        std::cerr << "Error: Failed to find key " << name
                  << " in BytecodeProgram::methods table\n";
    }
    return it->second;
}

void BytecodeProgram::print(std::ostream &os) const {
    for (const auto &method : methods) {
        method.second.print(os);
    }
}
