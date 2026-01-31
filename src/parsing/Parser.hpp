#pragma once

#include <deque>
#include <string>

#include "ast/Node.h"
#include "lexing/Lexer.hpp"
#include "lexing/Token.hpp"
#include "util/Expected.hpp"

namespace parsing {

struct ParseError {
    std::string message;
    lexing::SourceSpan span{};
};

template <typename T>
using Result = util::Expected<T, ParseError>;

class Parser {
  public:
    Parser(lexing::Lexer lexer, lexing::DiagnosticSink *sink);

    Result<Node *> parse_goal();

    bool has_errors() const;
    int error_count() const;

  private:
    const lexing::Token &peek(std::size_t n = 0);
    lexing::Token consume();
    bool match(lexing::TokenKind kind);
    Result<lexing::Token> expect(lexing::TokenKind kind,
                                std::string_view expected_label);

    void report_error(const lexing::Token &token,
                      std::string_view expected_label);

    Result<Node *> parse_main_class();
    Result<Node *> parse_class_decl();
    Result<Node *> parse_class_body();
    Result<Node *> parse_var_decl();
    Result<Node *> parse_method_decl();
    Result<Node *> parse_method_body();
    Result<Node *> parse_method_body_item();
    Result<Node *> parse_statement();
    Result<Node *> parse_statement_list();
    Result<Node *> parse_expression_list();
    Result<Node *> parse_type();
    Result<Node *> parse_identifier();
    Result<Node *> parse_integer();
    Result<Node *> parse_expression(int min_bp = 0);

    lexing::Lexer lexer_;
    lexing::DiagnosticSink *sink_ = nullptr;
    std::deque<lexing::Token> buffer_;
    int error_count_ = 0;
    bool reported_syntax_error_ = false;
};

} // namespace parsing
