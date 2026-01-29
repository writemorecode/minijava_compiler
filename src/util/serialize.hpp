#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <fstream>
#include <string>
#include <vector>

#include "bytecode/Opcode.hpp"

class Serializer {
  private:
    std::ofstream os;

  public:
    explicit Serializer(std::ofstream &stream) : os(std::move(stream)) {}

    void writeInteger(size_t value);
    void writeOpcode(Opcode value);
    void writeString(const std::string &str);
    void writeStringVector(const std::vector<std::string> &vec);
};

class Deserializer {
  private:
    std::ifstream is;

  public:
    explicit Deserializer(std::ifstream &stream) : is(std::move(stream)) {}

    size_t readInteger();
    Opcode readOpcode();
    std::string readString();
    std::vector<std::string> readStringVector();
};

#endif
