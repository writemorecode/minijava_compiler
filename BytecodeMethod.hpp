#ifndef BYTECODEMETHOD_HPP
#define BYTECODEMETHOD_HPP

#include "BytecodeMethodBlock.hpp"
#include "serialize.hpp"
#include <string>
#include <vector>

class BytecodeMethod {
    std::vector<BytecodeMethodBlock> blocks;

    std::string name;
    std::vector<std::string> variables;

  public:
    BytecodeMethod(const std::string &name_,
                   std::vector<std::string> variables_)
        : name(name_), variables(std::move(variables_)){};

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

    void serialize(Serializer &serializer) const;
};

#endif // BYTECODEMETHOD_HPP
