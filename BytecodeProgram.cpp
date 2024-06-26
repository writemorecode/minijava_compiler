#include "BytecodeProgram.hpp"

#include "BytecodeMethod.hpp"
#include "serialize.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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

    const auto &mainMethod = methods.front();
    mainMethod.serialize(serializer);

    serializer.writeInteger(methods.size() - 1);

    std::for_each(std::next(methods.cbegin()), methods.cend(),
                  [&serializer](const BytecodeMethod &method) {
                      method.serialize(serializer);
                  });
}
