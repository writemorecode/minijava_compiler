#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <fstream>
#include <string>
#include <vector>

class Serializer {
  private:
    std::ofstream os;

  public:
    explicit Serializer(std::ofstream &stream) : os(std::move(stream)) {}

    void writeInteger(size_t value);
    void writeString(const std::string &str);
    void writeStringVector(const std::vector<std::string> &vec);
};

class Deserializer {
  private:
    std::ifstream is;

  public:
    explicit Deserializer(std::ifstream &stream) : is(std::move(stream)) {}

    std::streamsize readInteger();
    std::string readString();
    std::vector<std::string> readStringVector();
};

/*
void writeInteger(std::ofstream &os, size_t value) {
    os.write(reinterpret_cast<const char *>(&value), sizeof(value));
}

void writeString(std::ofstream &os, const std::string &str) {
    writeInteger(os, str.size());
    os << str;
}

void writeStringVector(std::ofstream &os, const std::vector<std::string> &vec) {
    writeInteger(os, vec.size());
    for (const auto &str : vec) {
        writeString(os, str);
    }
}

size_t readInteger(std::ifstream &is) {
    size_t value = 0;
    is.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

std::string readString(std::ifstream &is) {
    const auto length = readInteger(is);
    std::string str(length, '\0');
    is.read(str.data(), length);
    return str;
}

std::vector<std::string> readStringVector(std::ifstream &is) {
    const auto length = readInteger(is);
    std::vector<std::string> vec;
    vec.reserve(length);
    for (size_t i = 0; i < length; i++) {
        vec.emplace_back(readString(is));
    }
    return vec;
}

*/

#endif
