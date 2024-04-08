#include "BytecodeProgram.hpp"

#include "serialize.hpp"

#include <fstream>
#include <iostream>
#include <string>

BytecodeMethod &
BytecodeProgram::addBytecodeMethod(const std::string &name,
                                   std::vector<std::string> variables) {
    auto it =
        methods.insert({name, BytecodeMethod{name, std::move(variables)}});
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
    Serializer serializer(os);
    serializer.writeInteger(methods.size());

    for (const auto &[name, method] : methods) {
        serializer.writeString(name);
        serializer.writeStringVector(method.getVariables());
    }
}
