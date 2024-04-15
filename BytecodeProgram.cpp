#include "BytecodeProgram.hpp"

#include "serialize.hpp"

#include <fstream>
#include <iostream>
#include <string>

BytecodeMethod &
BytecodeProgram::addBytecodeMethod(const std::string &name,
                                   std::vector<std::string> variables) {
    methods.push_back(BytecodeMethod(name, std::move(variables)));
    return methods.back();
}

BytecodeMethod &BytecodeProgram::getBytecodeMethod(const std::string &name) {
    const auto &it = std::find(methods.begin(), methods.end(), name);
    if (it == methods.end()) {
        std::cerr << "Error: Failed to find key " << name
                  << " in BytecodeProgram::methods table\n";
    }
    return *it;
}

void BytecodeProgram::print(std::ostream &os) const {
    for (const auto &method : methods) {
        method.print(os);
    }
}

void BytecodeProgram::serialize(std::ofstream &os) const {
    Serializer serializer(os);
    serializer.writeInteger(methods.size());

    for (const auto &method : methods) {
        method.serialize(serializer);
    }
}
