#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace lexing {

struct SourceLocation {
    std::size_t offset = 0;
    std::size_t line = 1;
    std::size_t column = 1;
};

struct SourceSpan {
    SourceLocation begin{};
    SourceLocation end{};
};

enum class Severity : std::uint8_t { Note, Warning, Error };

struct Diagnostic {
    Severity severity{};
    std::string message;
    SourceSpan span{};
};

class DiagnosticSink {
  public:
    virtual ~DiagnosticSink() = default;
    virtual void emit(Diagnostic d) = 0;
};

} // namespace lexing
