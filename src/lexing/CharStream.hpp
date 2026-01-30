#pragma once

#include <cstddef>

#include "lexing/Token.hpp"

namespace lexing {

class CharStream {
  public:
    virtual ~CharStream() = default;

    virtual bool eof() const = 0;
    virtual char peek(std::size_t lookahead = 0) const = 0;
    virtual char get() = 0;
    virtual SourceLocation location() const = 0;
};

} // namespace lexing
