#include "BytecodeProgram.hpp"

#include <fstream>
#include <iostream>
#include <string>

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
    auto writeIntegerToStream = [&os](size_t value) {
        os.write(reinterpret_cast<const char *>(&value), sizeof(value));
    };
    auto writeStringToStream = [&](const std::string &str) {
        writeIntegerToStream(str.size());
        os << str;
    };

    writeIntegerToStream(methods.size());

    for (const auto &method : methods) {
        const auto &name = method.first;
        writeStringToStream(name);
    }
}
