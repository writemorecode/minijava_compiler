#include "serialize.hpp"

void Serializer::writeInteger(size_t value) {
    os.write(reinterpret_cast<const char *>(&value), sizeof(value));
}
void Serializer::writeOpcode(Opcode value) {
    os.write(reinterpret_cast<const char *>(&value), sizeof(value));
}
void Serializer::writeString(const std::string &str) {
    writeInteger(str.size());
    os << str;
}
void Serializer::writeStringVector(const std::vector<std::string> &vec) {
    writeInteger(vec.size());
    for (const auto &str : vec) {
        writeString(str);
    }
}

size_t Deserializer::readInteger() {
    size_t value = 0;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}
std::string Deserializer::readString() {
    const auto length = readInteger();
    std::string str(length, '\0');
    is.read(str.data(), length);
    return str;
}
std::vector<std::string> Deserializer::readStringVector() {
    const auto length = readInteger();
    std::vector<std::string> vec;
    vec.reserve(length);
    for (auto i = 0; i < length; i++) {
        vec.emplace_back(readString());
    }
    return vec;
}

Opcode Deserializer::readOpcode() {
    Opcode value = Opcode::ADD;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}
