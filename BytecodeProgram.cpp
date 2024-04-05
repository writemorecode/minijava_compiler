#include "BytecodeProgram.hpp"
#include "SymbolTable.hpp"

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
    auto writeIntegerToStream = [&os](size_t value) {
        os.write(reinterpret_cast<const char *>(&value), sizeof(value));
    };
    auto writeStringToStream = [&](const std::string &str) {
        writeIntegerToStream(str.size());
        os << str;
    };
    auto writeStringVectorToStream = [&](const std::vector<std::string> &vec) {
        writeIntegerToStream(vec.size());
        for (const auto &str : vec) {
            writeStringToStream(str);
        }
    };

    writeIntegerToStream(methods.size());

    for (const auto &[name, method] : methods) {
        writeStringToStream(name);
        const auto methodVariables = method.getVariables();
        writeStringVectorToStream(methodVariables);
    }
}
