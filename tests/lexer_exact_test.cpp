#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "lexing/Lexer.hpp"
#include "lexing/StringViewStream.hpp"
#include "lexing/Token.hpp"

namespace {

class CollectingDiagnosticSink final : public lexing::DiagnosticSink {
  public:
    void emit(lexing::Diagnostic d) override { diagnostics.push_back(d); }

    std::vector<lexing::Diagnostic> diagnostics;
};

struct ExpectedToken {
    lexing::TokenKind kind;
    std::string lexeme;
    std::optional<std::int64_t> int_value;
};

ExpectedToken
expect_token(lexing::TokenKind kind, std::string_view lexeme,
             std::optional<std::int64_t> int_value = std::nullopt) {
    return ExpectedToken{
        .kind = kind, .lexeme = std::string(lexeme), .int_value = int_value};
}

void assert_no_errors(const std::vector<lexing::Diagnostic> &diagnostics) {
    int error_count = 0;
    for (const auto &d : diagnostics) {
        if (d.severity == lexing::Severity::Error) {
            error_count += 1;
        }
    }
    ASSERT_EQ(error_count, 0)
        << "Lexer reported " << error_count << " error(s).";
}

} // namespace

TEST(LexerExact, AllTokens) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    int[] arr;
    int a;
    int b;
    boolean flag;
    Foo foo;
    a = 1 + 2 - 3 * 4 / 5;
    b = 0;
    flag = true;
    if (!flag && a < b || a > b) {
      b = a;
    } else {
      flag = false;
    }
    while (b < 10) {
      b = b + 1;
    }
    arr = new int[7];
    a = arr.length;
    foo = new Foo();
    a = foo.bar(8, 9);
    System.out.println(a);
  }
}

class Foo {
  public Foo bar(int x, int y) {
    if (x == y) {
      return this;
    } else {
      return new Foo();
    }
  }
}
)";

    CollectingDiagnosticSink diag;
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &diag);

    using TK = lexing::TokenKind;
    const std::vector<ExpectedToken> expected = {
        expect_token(TK::KwPublic, "public"),
        expect_token(TK::KwClass, "class"),
        expect_token(TK::Identifier, "Main"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwPublic, "public"),
        expect_token(TK::KwStatic, "static"),
        expect_token(TK::KwVoid, "void"),
        expect_token(TK::KwMain, "main"),
        expect_token(TK::LParen, "("),
        expect_token(TK::KwString, "String"),
        expect_token(TK::LSquare, "["),
        expect_token(TK::RSquare, "]"),
        expect_token(TK::Identifier, "args"),
        expect_token(TK::RParen, ")"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::LSquare, "["),
        expect_token(TK::RSquare, "]"),
        expect_token(TK::Identifier, "arr"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::KwBoolean, "boolean"),
        expect_token(TK::Identifier, "flag"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "Foo"),
        expect_token(TK::Identifier, "foo"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Assign, "="),
        expect_token(TK::IntLiteral, "1", 1),
        expect_token(TK::Plus, "+"),
        expect_token(TK::IntLiteral, "2", 2),
        expect_token(TK::Minus, "-"),
        expect_token(TK::IntLiteral, "3", 3),
        expect_token(TK::Star, "*"),
        expect_token(TK::IntLiteral, "4", 4),
        expect_token(TK::Slash, "/"),
        expect_token(TK::IntLiteral, "5", 5),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Assign, "="),
        expect_token(TK::IntLiteral, "0", 0),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "flag"),
        expect_token(TK::Assign, "="),
        expect_token(TK::KwTrue, "true"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::KwIf, "if"),
        expect_token(TK::LParen, "("),
        expect_token(TK::Bang, "!"),
        expect_token(TK::Identifier, "flag"),
        expect_token(TK::AndAnd, "&&"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Lt, "<"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::OrOr, "||"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Gt, ">"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::RParen, ")"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Assign, "="),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::KwElse, "else"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::Identifier, "flag"),
        expect_token(TK::Assign, "="),
        expect_token(TK::KwFalse, "false"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::KwWhile, "while"),
        expect_token(TK::LParen, "("),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Lt, "<"),
        expect_token(TK::IntLiteral, "10", 10),
        expect_token(TK::RParen, ")"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Assign, "="),
        expect_token(TK::Identifier, "b"),
        expect_token(TK::Plus, "+"),
        expect_token(TK::IntLiteral, "1", 1),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::Identifier, "arr"),
        expect_token(TK::Assign, "="),
        expect_token(TK::KwNew, "new"),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::LSquare, "["),
        expect_token(TK::IntLiteral, "7", 7),
        expect_token(TK::RSquare, "]"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Assign, "="),
        expect_token(TK::Identifier, "arr"),
        expect_token(TK::Dot, "."),
        expect_token(TK::KwLength, "length"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "foo"),
        expect_token(TK::Assign, "="),
        expect_token(TK::KwNew, "new"),
        expect_token(TK::Identifier, "Foo"),
        expect_token(TK::LParen, "("),
        expect_token(TK::RParen, ")"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::Assign, "="),
        expect_token(TK::Identifier, "foo"),
        expect_token(TK::Dot, "."),
        expect_token(TK::Identifier, "bar"),
        expect_token(TK::LParen, "("),
        expect_token(TK::IntLiteral, "8", 8),
        expect_token(TK::Comma, ","),
        expect_token(TK::IntLiteral, "9", 9),
        expect_token(TK::RParen, ")"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::KwPrintln, "System.out.println"),
        expect_token(TK::LParen, "("),
        expect_token(TK::Identifier, "a"),
        expect_token(TK::RParen, ")"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::KwClass, "class"),
        expect_token(TK::Identifier, "Foo"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwPublic, "public"),
        expect_token(TK::Identifier, "Foo"),
        expect_token(TK::Identifier, "bar"),
        expect_token(TK::LParen, "("),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::Identifier, "x"),
        expect_token(TK::Comma, ","),
        expect_token(TK::KwInt, "int"),
        expect_token(TK::Identifier, "y"),
        expect_token(TK::RParen, ")"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwIf, "if"),
        expect_token(TK::LParen, "("),
        expect_token(TK::Identifier, "x"),
        expect_token(TK::EqEq, "=="),
        expect_token(TK::Identifier, "y"),
        expect_token(TK::RParen, ")"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwReturn, "return"),
        expect_token(TK::KwThis, "this"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::KwElse, "else"),
        expect_token(TK::LCurly, "{"),
        expect_token(TK::KwReturn, "return"),
        expect_token(TK::KwNew, "new"),
        expect_token(TK::Identifier, "Foo"),
        expect_token(TK::LParen, "("),
        expect_token(TK::RParen, ")"),
        expect_token(TK::Semi, ";"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::RCurly, "}"),
        expect_token(TK::Eof, ""),
    };

    std::vector<lexing::Token> actual;
    while (true) {
        lexing::Token token = lexer.next();
        actual.push_back(token);
        if (token.kind == TK::Eof) {
            break;
        }
    }

    assert_no_errors(diag.diagnostics);

    ASSERT_EQ(actual.size(), expected.size())
        << "Expected " << expected.size() << " tokens, got " << actual.size()
        << ".";

    for (std::size_t i = 0; i < expected.size(); ++i) {
        SCOPED_TRACE(::testing::Message() << "Token index " << i);
        EXPECT_EQ(actual[i].kind, expected[i].kind);
        EXPECT_EQ(std::string(actual[i].lexeme), expected[i].lexeme);
        if (expected[i].int_value.has_value()) {
            ASSERT_TRUE(std::holds_alternative<std::int64_t>(actual[i].value));
            EXPECT_EQ(std::get<std::int64_t>(actual[i].value),
                      expected[i].int_value.value());
        } else {
            EXPECT_TRUE(
                std::holds_alternative<std::monostate>(actual[i].value));
        }
    }
}
