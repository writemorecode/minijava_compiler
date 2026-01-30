#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

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

enum class TokenKind : std::uint16_t {
    Eof,
    Invalid,

    Identifier,
    IntLiteral,

    KwPublic,
    KwStatic,
    KwVoid,
    KwMain,
    KwString,
    KwInt,
    KwBoolean,
    KwIf,
    KwElse,
    KwWhile,
    KwPrintln,
    KwLength,
    KwTrue,
    KwFalse,
    KwThis,
    KwNew,
    KwReturn,
    KwClass,

    LParen,
    RParen,
    LCurly,
    RCurly,
    LSquare,
    RSquare,
    Semi,
    Comma,
    Dot,

    Plus,
    Minus,
    Star,
    Slash,
    Assign,
    EqEq,
    Lt,
    Gt,
    AndAnd,
    OrOr,
    Bang,
};

using TokenValue = std::variant<std::monostate, std::int64_t>;

struct Token {
    TokenKind kind = TokenKind::Invalid;
    std::string_view lexeme{};
    SourceSpan span{};
    TokenValue value{};
};

} // namespace lexing
