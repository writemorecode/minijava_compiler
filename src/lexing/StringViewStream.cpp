#include "lexing/StringViewStream.hpp"
#include <cstddef>
#include <string_view>

namespace lexing {

StringViewStream::StringViewStream(std::string_view input) : input_(input) {}

bool StringViewStream::eof() const { return index_ >= input_.size(); }

char StringViewStream::peek(std::size_t lookahead) const {
    const std::size_t pos = index_ + lookahead;
    if (pos >= input_.size()) {
        return '\0';
    }
    return input_[pos];
}

char StringViewStream::get() {
    if (eof()) {
        return '\0';
    }

    const char ch = input_[index_++];
    loc_.offset += 1;
    if (ch == '\n') {
        loc_.line += 1;
        loc_.column = 1;
    } else {
        loc_.column += 1;
    }
    return ch;
}

SourceLocation StringViewStream::location() const { return loc_; }

} // namespace lexing
