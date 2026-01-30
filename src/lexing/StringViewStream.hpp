#pragma once

#include <string_view>

#include "lexing/CharStream.hpp"

namespace lexing {

class StringViewStream final : public CharStream {
  public:
    explicit StringViewStream(std::string_view input);

    bool eof() const override;
    char peek(std::size_t lookahead = 0) const override;
    char get() override;
    SourceLocation location() const override;

  private:
    std::string_view input_;
    std::size_t index_ = 0;
    SourceLocation loc_{};
};

} // namespace lexing
