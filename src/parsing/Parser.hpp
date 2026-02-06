#pragma once

#include <deque>
#include <expected>
#include <memory>
#include <string>

#include "ast/Node.h"
#include "lexing/Lexer.hpp"
#include "lexing/Token.hpp"
namespace parsing {

struct ParseError {
    std::string message;
    lexing::SourceSpan span{};
};

template <typename T> using Result = std::expected<T, ParseError>;

class Parser {
  public:
    Parser(lexing::Lexer lexer, lexing::DiagnosticSink *sink);

    Result<std::unique_ptr<Node>> parse_goal();

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

    Result<std::unique_ptr<Node>> parse_main_class();
    Result<std::unique_ptr<Node>> parse_class_decl();
    Result<std::unique_ptr<Node>> parse_class_decl_list();
    Result<std::unique_ptr<Node>> parse_class_body();
    Result<std::unique_ptr<Node>> parse_class_var_decl_list();
    Result<std::unique_ptr<Node>> parse_method_decl_list();
    Result<std::unique_ptr<Node>> parse_var_decl();
    Result<std::unique_ptr<Node>> parse_method_decl();
    Result<std::unique_ptr<Node>> parse_method_parameter();
    Result<std::unique_ptr<Node>> parse_method_parameter_list();
    Result<std::unique_ptr<Node>> parse_method_body();
    Result<std::unique_ptr<Node>> parse_method_body_item();
    Result<std::unique_ptr<Node>> parse_method_body_item_list();
    Result<std::unique_ptr<Node>> parse_statement();
    Result<std::unique_ptr<Node>> parse_statement_list();
    Result<std::unique_ptr<Node>> parse_expression_list();
    Result<std::unique_ptr<Node>> parse_type();
    Result<std::unique_ptr<Node>> parse_identifier();
    Result<std::unique_ptr<Node>> parse_integer();
    Result<std::unique_ptr<Node>> parse_expression(int min_bp = 0);

    lexing::Lexer lexer_;
    lexing::DiagnosticSink *sink_ = nullptr;
    std::deque<lexing::Token> buffer_;
    int error_count_ = 0;
    bool reported_syntax_error_ = false;
};

} // namespace parsing
