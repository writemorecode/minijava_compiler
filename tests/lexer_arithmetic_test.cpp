#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
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

std::string load_valid_source(const std::string &filename) {
    const std::filesystem::path test_root = TEST_FILES_ROOT;
    const std::filesystem::path path = test_root / "valid" / filename;

    EXPECT_TRUE(std::filesystem::exists(path))
        << "Test file not found at: " << path;

    std::ifstream input(path, std::ios::in);
    EXPECT_TRUE(input.good()) << "Failed to open test file: " << path;

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

::testing::AssertionResult lexes_without_errors(const std::string &source) {
    if (source.empty()) {
        return ::testing::AssertionFailure() << "Source was empty.";
    }

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

    if (saw_invalid) {
        return ::testing::AssertionFailure()
               << "Lexer produced invalid tokens.";
    }

    int error_count = 0;
    for (const auto &d : diag.diagnostics) {
        if (d.severity == lexing::Severity::Error) {
            error_count += 1;
        }
    }

    if (error_count != 0) {
        return ::testing::AssertionFailure()
               << "Lexer reported " << error_count << " error(s).";
    }

    return ::testing::AssertionSuccess();
}

} // namespace

TEST(LexerValid, Arithmetic) {
    std::string source = load_valid_source("Arithmetic.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, BinarySearch) {
    std::string source = load_valid_source("BinarySearch.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, BinaryTree) {
    std::string source = load_valid_source("BinaryTree.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, BubbleSort) {
    std::string source = load_valid_source("BubbleSort.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, Factorial) {
    std::string source = load_valid_source("Factorial.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, LinearSearch) {
    std::string source = load_valid_source("LinearSearch.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, LinkedList) {
    std::string source = load_valid_source("LinkedList.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, QuickSort) {
    std::string source = load_valid_source("QuickSort.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, SemanticArrayInteger) {
    std::string source = load_valid_source("SemanticArrayInteger.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, SemanticDuplicateIdentifiersSeparateScope) {
    std::string source =
        load_valid_source("SemanticDuplicateIdentifiersSeparateScope.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, SemanticMethodCallInBooleanExpression) {
    std::string source =
        load_valid_source("SemanticMethodCallInBooleanExpression.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}

TEST(LexerValid, SemanticValidMethodCall) {
    std::string source = load_valid_source("SemanticValidMethodCall.java");
    ASSERT_FALSE(source.empty());
    EXPECT_TRUE(lexes_without_errors(source));
}
