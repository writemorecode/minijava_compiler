#include <gtest/gtest.h>

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "lexing/Lexer.hpp"
#include "lexing/StringViewStream.hpp"
#include "lexing/Token.hpp"
#include "parsing/Parser.hpp"

namespace {

class CollectingDiagnosticSink final : public lexing::DiagnosticSink {
  public:
    void emit(lexing::Diagnostic d) override { diagnostics.push_back(d); }

    std::vector<lexing::Diagnostic> diagnostics;
};

struct SyntaxErrorCase {
    std::string file_name;
    parsing::ParseErrorKind kind;
    std::optional<lexing::TokenKind> expected_token;
    std::size_t expected_line;
};

std::string load_file(const std::filesystem::path &path) {
    std::ifstream in(path, std::ios::binary);
    EXPECT_TRUE(in.is_open()) << "Failed to open file: " << path;
    if (!in.is_open()) {
        return {};
    }

    std::ostringstream out;
    out << in.rdbuf();
    return out.str();
}

int count_error_diagnostics(
    const std::vector<lexing::Diagnostic> &diagnostics) {
    int count = 0;
    for (const auto &d : diagnostics) {
        if (d.severity == lexing::Severity::Error) {
            count += 1;
        }
    }
    return count;
}

std::string token_name(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::Eof:
        return "Eof";
    case TokenKind::Invalid:
        return "Invalid";
    case TokenKind::Identifier:
        return "Identifier";
    case TokenKind::IntLiteral:
        return "IntLiteral";
    case TokenKind::KwPublic:
        return "KwPublic";
    case TokenKind::KwStatic:
        return "KwStatic";
    case TokenKind::KwVoid:
        return "KwVoid";
    case TokenKind::KwMain:
        return "KwMain";
    case TokenKind::KwString:
        return "KwString";
    case TokenKind::KwInt:
        return "KwInt";
    case TokenKind::KwBoolean:
        return "KwBoolean";
    case TokenKind::KwIf:
        return "KwIf";
    case TokenKind::KwElse:
        return "KwElse";
    case TokenKind::KwWhile:
        return "KwWhile";
    case TokenKind::KwPrintln:
        return "KwPrintln";
    case TokenKind::KwLength:
        return "KwLength";
    case TokenKind::KwTrue:
        return "KwTrue";
    case TokenKind::KwFalse:
        return "KwFalse";
    case TokenKind::KwThis:
        return "KwThis";
    case TokenKind::KwNew:
        return "KwNew";
    case TokenKind::KwReturn:
        return "KwReturn";
    case TokenKind::KwClass:
        return "KwClass";
    case TokenKind::LParen:
        return "LParen";
    case TokenKind::RParen:
        return "RParen";
    case TokenKind::LCurly:
        return "LCurly";
    case TokenKind::RCurly:
        return "RCurly";
    case TokenKind::LSquare:
        return "LSquare";
    case TokenKind::RSquare:
        return "RSquare";
    case TokenKind::Semi:
        return "Semi";
    case TokenKind::Comma:
        return "Comma";
    case TokenKind::Dot:
        return "Dot";
    case TokenKind::Plus:
        return "Plus";
    case TokenKind::Minus:
        return "Minus";
    case TokenKind::Star:
        return "Star";
    case TokenKind::Slash:
        return "Slash";
    case TokenKind::Assign:
        return "Assign";
    case TokenKind::EqEq:
        return "EqEq";
    case TokenKind::Lt:
        return "Lt";
    case TokenKind::Gt:
        return "Gt";
    case TokenKind::AndAnd:
        return "AndAnd";
    case TokenKind::OrOr:
        return "OrOr";
    case TokenKind::Bang:
        return "Bang";
    }
    return "Unknown";
}

std::string parse_error_kind_name(parsing::ParseErrorKind kind) {
    switch (kind) {
    case parsing::ParseErrorKind::ExpectedToken:
        return "ExpectedToken";
    case parsing::ParseErrorKind::ExpectedExpression:
        return "ExpectedExpression";
    case parsing::ParseErrorKind::ExpectedStatement:
        return "ExpectedStatement";
    case parsing::ParseErrorKind::ExpectedType:
        return "ExpectedType";
    }
    return "Unknown";
}

std::string
case_name(const testing::TestParamInfo<SyntaxErrorCase> &param_info) {
    std::string name;
    name.reserve(param_info.param.file_name.size());
    for (char c : param_info.param.file_name) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')) {
            name.push_back(c);
        } else {
            name.push_back('_');
        }
    }
    return name;
}

class ParserSyntaxErrorFilesTest
    : public testing::TestWithParam<SyntaxErrorCase> {};

TEST_P(ParserSyntaxErrorFilesTest, ReportsExpectedParseError) {
    const SyntaxErrorCase &test_case = GetParam();
    const auto fixture_path = std::filesystem::path(TEST_FILES_ROOT) /
                              "syntax_errors" / test_case.file_name;

    const std::string source = load_file(fixture_path);
    ASSERT_FALSE(source.empty())
        << "Input fixture should not be empty: " << fixture_path;

    CollectingDiagnosticSink diag;
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &diag);
    parsing::Parser parser(std::move(lexer), &diag);

    auto parse_result = parser.parse_goal();

    ASSERT_FALSE(parse_result.has_value())
        << "Expected parse error for fixture: " << fixture_path;

    const parsing::ParseError &error = parse_result.error();
    EXPECT_EQ(error.kind, test_case.kind)
        << "Fixture: " << fixture_path
        << ", expected kind: " << parse_error_kind_name(test_case.kind);
    EXPECT_EQ(error.expected_token, test_case.expected_token)
        << "Fixture: " << fixture_path << ", expected token: "
        << (test_case.expected_token.has_value()
                ? token_name(test_case.expected_token.value())
                : std::string("nullopt"));
    EXPECT_EQ(error.span.begin.line, test_case.expected_line)
        << "Fixture: " << fixture_path;
    EXPECT_EQ(count_error_diagnostics(diag.diagnostics), 1)
        << "Fixture: " << fixture_path;
}

INSTANTIATE_TEST_SUITE_P(
    SyntaxErrorFixtures, ParserSyntaxErrorFilesTest,
    testing::Values(SyntaxErrorCase{"ClassMissingClosingBracket.java",
                                    parsing::ParseErrorKind::ExpectedType,
                                    std::nullopt, 8},
                    SyntaxErrorCase{"ExtraSemicolon.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::RCurly, 5},
                    SyntaxErrorCase{"IllegalClassName.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::Identifier, 7},
                    SyntaxErrorCase{"IllegalIfInIfCondition.java",
                                    parsing::ParseErrorKind::ExpectedExpression,
                                    std::nullopt, 9},
                    SyntaxErrorCase{"InvalidClassSignature.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::LCurly, 7},
                    SyntaxErrorCase{"InvalidMethodCall.java",
                                    parsing::ParseErrorKind::ExpectedExpression,
                                    std::nullopt, 11},
                    SyntaxErrorCase{"InvalidMethodCall2.java",
                                    parsing::ParseErrorKind::ExpectedExpression,
                                    std::nullopt, 11},
                    SyntaxErrorCase{"InvalidMethodCall3.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::LParen, 11},
                    SyntaxErrorCase{"InvalidMethodDeclaration.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::Identifier, 8},
                    SyntaxErrorCase{"InvalidPrintStatement.java",
                                    parsing::ParseErrorKind::ExpectedExpression,
                                    std::nullopt, 9},
                    SyntaxErrorCase{"InvalidTypeDeclaration.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::Identifier, 9},
                    SyntaxErrorCase{"MethodMissingClosingBracket.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::RCurly, 12},
                    SyntaxErrorCase{"MissingMethodBraces.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::LCurly, 5},
                    SyntaxErrorCase{"MissingReturnStatement.java",
                                    parsing::ParseErrorKind::ExpectedStatement,
                                    std::nullopt, 13},
                    SyntaxErrorCase{"MissingStatementInMain.java",
                                    parsing::ParseErrorKind::ExpectedStatement,
                                    std::nullopt, 3},
                    SyntaxErrorCase{"NoMainMethod.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::KwStatic, 2},
                    SyntaxErrorCase{"StatementAfterReturn.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::RCurly, 12},
                    SyntaxErrorCase{"UnbalancedClassBraces.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::RCurly, 4},
                    SyntaxErrorCase{"UnmatchedParentheses.java",
                                    parsing::ParseErrorKind::ExpectedToken,
                                    lexing::TokenKind::RParen, 10}),
    case_name);

} // namespace
