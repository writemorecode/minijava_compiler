#include "BytecodeProgram.hpp"

#include <fstream>
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

void BytecodeProgram::serialize(std::ofstream &os) const {
    const size_t methodCount = methods.size();
    os.write(reinterpret_cast<const char *>(&methodCount), sizeof(methodCount));

    for (const auto &method : methods) {
        const auto &name = method.first;
        const size_t nameLength = name.size();
        os.write(reinterpret_cast<const char *>(&nameLength),
                 sizeof(nameLength));
        os.write(name.data(), nameLength);
    }
}
