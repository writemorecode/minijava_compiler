#ifndef BYTECODEMETHOD_HPP
#define BYTECODEMETHOD_HPP

#include "bytecode/BytecodeMethodBlock.hpp"
#include "util/serialize.hpp"
#include <string>
#include <vector>

class BytecodeMethod {
    std::vector<BytecodeMethodBlock> blocks;

    std::string name;
    std::vector<std::string> variables;
    std::vector<std::string> fieldVariables;

  public:
    BytecodeMethod(const std::string &name_,
                   std::vector<std::string> variables_,
                   std::vector<std::string> fieldVariables_)
        : name(name_), variables(std::move(variables_)),
          fieldVariables(std::move(fieldVariables_)) {};

    bool operator==(const std::string &otherName) const {
        return name == otherName;
    }

    [[nodiscard]] BytecodeMethodBlock &
    addBytecodeMethodBlock(const std::string &name);

    [[nodiscard]] BytecodeMethodBlock &
    getBytecodeMethodBlock(const std::string &name);

    BytecodeMethodBlock &getFirstBlock();

    void print(std::ostream &os) const;

    [[nodiscard]] const auto &getVariables() const { return variables; }
    [[nodiscard]] const auto &getFieldVariables() const {
        return fieldVariables;
    }

    void serialize(Serializer &serializer) const;
};

#endif // BYTECODEMETHOD_HPP
