#pragma once

#include <cstdint>
#include <string_view>
#include <variant>

#include "lexing/Diagnostics.hpp"

namespace lexing {

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
