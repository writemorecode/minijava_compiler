#include "BytecodeProgram.hpp"

void BytecodeProgram::addMethod(const std::string &name) {
    methods.insert(std::make_pair(name, BytecodeMethod()));
}

BytecodeMethod &BytecodeProgram::getMethod(const std::string &name) {
    const auto &it = methods.find(name);
    return it->second;
}

void BytecodeProgram::print(std::ostream &os) const {
    for (const auto &method : methods) {
        os << method.first << '\n';
        method.second.print(os);
    }
}
