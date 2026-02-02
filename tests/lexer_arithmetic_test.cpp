#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
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

std::string read_file(const std::filesystem::path &path) {
    std::ifstream input(path, std::ios::in);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

} // namespace

TEST(Lexer, ArithmeticFileLexesToExpectedTokens) {
    const std::filesystem::path test_root = TEST_FILES_ROOT;
    const std::filesystem::path arithmetic =
        test_root / "valid" / "Arithmetic.java";

    ASSERT_TRUE(std::filesystem::exists(arithmetic))
        << "Test file not found at: " << arithmetic;

    std::string source = read_file(arithmetic);
    ASSERT_FALSE(source.empty()) << "Arithmetic.java was empty.";

    CollectingDiagnosticSink diag;
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &diag);

    bool saw_invalid = false;
    while (true) {
        lexing::Token token = lexer.next();
        if (token.kind == lexing::TokenKind::Invalid) {
            saw_invalid = true;
        }
        if (token.kind == lexing::TokenKind::Eof) {
            break;
        }
    }

    int error_count = 0;
    for (const auto &d : diag.diagnostics) {
        if (d.severity == lexing::Severity::Error) {
            error_count += 1;
        }
    }

    EXPECT_FALSE(saw_invalid) << "Lexer produced invalid tokens.";
    EXPECT_EQ(error_count, 0) << "Lexer reported errors.";
}
