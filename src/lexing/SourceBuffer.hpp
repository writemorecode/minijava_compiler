#pragma once

#include <fstream>
#include <optional>
#include <string>
#include <string_view>

namespace lexing {

class SourceBuffer {
  public:
    static std::optional<SourceBuffer> from_file(const std::string &path,
                                                 std::string &error) {
        std::ifstream in(path, std::ios::binary);
        if (!in.is_open()) {
            error = "Failed to open file: " + path;
            return std::nullopt;
        }

        in.seekg(0, std::ios::end);
        const std::streampos size = in.tellg();
        if (size < 0) {
            error = "Failed to read file: " + path;
            return std::nullopt;
        }

        std::string data;
        data.resize(static_cast<std::size_t>(size));
        in.seekg(0, std::ios::beg);
        if (!in.read(data.data(), size)) {
            error = "Failed to read file: " + path;
            return std::nullopt;
        }

        return SourceBuffer(std::move(data));
    }

    static SourceBuffer from_string(std::string data) {
        return SourceBuffer(std::move(data));
    }

    std::string_view view() const { return data_; }
    const std::string &str() const { return data_; }

  private:
    explicit SourceBuffer(std::string data) : data_(std::move(data)) {}

    std::string data_{};
};

} // namespace lexing
