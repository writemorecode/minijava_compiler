#include "parsing/Parser.hpp"

#include <cassert>
#include <deque>
#include <string>

namespace parsing {

namespace {
std::string token_debug_name(const lexing::Token &token) {
    using lexing::TokenKind;
    switch (token.kind) {
    case TokenKind::Eof:
        return "end of file";
    case TokenKind::Invalid:
        return "invalid token";
    case TokenKind::Identifier:
        return "ID";
    case TokenKind::IntLiteral:
        return "INT_LITERAL";
    case TokenKind::KwPublic:
        return "PUBLIC";
    case TokenKind::KwStatic:
        return "STATIC";
    case TokenKind::KwVoid:
        return "VOID";
    case TokenKind::KwMain:
        return "MAIN";
    case TokenKind::KwString:
        return "STRING";
    case TokenKind::KwInt:
        return "INT";
    case TokenKind::KwBoolean:
        return "BOOL";
    case TokenKind::KwIf:
        return "IF";
    case TokenKind::KwElse:
        return "ELSE";
    case TokenKind::KwWhile:
        return "WHILE";
    case TokenKind::KwPrintln:
        return "PRINTLN";
    case TokenKind::KwLength:
        return "LENGTH";
    case TokenKind::KwTrue:
        return "TRUE";
    case TokenKind::KwFalse:
        return "FALSE";
    case TokenKind::KwThis:
        return "THIS";
    case TokenKind::KwNew:
        return "NEW";
    case TokenKind::KwReturn:
        return "RETURN";
    case TokenKind::KwClass:
        return "CLASS";
    case TokenKind::LParen:
        return "L_PAREN";
    case TokenKind::RParen:
        return "R_PAREN";
    case TokenKind::LCurly:
        return "L_CURLY";
    case TokenKind::RCurly:
        return "R_CURLY";
    case TokenKind::LSquare:
        return "L_SQUARE";
    case TokenKind::RSquare:
        return "R_SQUARE";
    case TokenKind::Semi:
        return "SEMI";
    case TokenKind::Comma:
        return "COMMA";
    case TokenKind::Dot:
        return "DOT";
    case TokenKind::Plus:
        return "PLUSOP";
    case TokenKind::Minus:
        return "MINUSOP";
    case TokenKind::Star:
        return "MULTOP";
    case TokenKind::Slash:
        return "DIVOP";
    case TokenKind::Assign:
        return "ASSIGN";
    case TokenKind::EqEq:
        return "EQ";
    case TokenKind::Lt:
        return "LT";
    case TokenKind::Gt:
        return "GT";
    case TokenKind::AndAnd:
        return "AND";
    case TokenKind::OrOr:
        return "OR";
    case TokenKind::Bang:
        return "NOT";
    }
    return "unknown token";
}
} // namespace

Parser::Parser(lexing::Lexer lexer, lexing::DiagnosticSink *sink)
    : lexer_(std::move(lexer)), sink_(sink) {}

bool Parser::has_errors() const {
    return error_count_ > 0;
}

int Parser::error_count() const {
    return error_count_;
}

const lexing::Token &Parser::peek(std::size_t n) {
    while (buffer_.size() <= n) {
        lexing::Token token = lexer_.next();
        if (token.kind == lexing::TokenKind::Invalid) {
            continue;
        }
        buffer_.push_back(token);
        if (token.kind == lexing::TokenKind::Eof) {
            break;
        }
    }
    return buffer_.at(n);
}

lexing::Token Parser::consume() {
    const lexing::Token &token = peek();
    lexing::Token result = token;
    buffer_.pop_front();
    return result;
}

bool Parser::match(lexing::TokenKind kind) {
    if (peek().kind == kind) {
        consume();
        return true;
    }
    return false;
}

Result<lexing::Token> Parser::expect(lexing::TokenKind kind,
                                    std::string_view expected_label) {
    if (peek().kind == kind) {
        return Result<lexing::Token>::ok(consume());
    }
    report_error(peek(), expected_label);
    return Result<lexing::Token>::err(
        ParseError{std::string(expected_label), peek().span});
}

void Parser::report_error(const lexing::Token &token,
                          std::string_view expected_label) {
    if (reported_syntax_error_) {
        return;
    }

    reported_syntax_error_ = true;
    error_count_ += 1;

    std::string err = "syntax error";
    if (!expected_label.empty()) {
        err += ", unexpected ";
        err += token_debug_name(token);
        err += ", expecting ";
        err += expected_label;
    }

    std::string message = "Syntax errors found! See the logs below:\n";
    message += "\t@error at line ";
    message += std::to_string(token.span.begin.line);
    message += ". Cannot generate a syntax for this input:";
    message += err;
    message += "\nEnd of syntax errors!\n";

    if (sink_ != nullptr) {
        sink_->emit({lexing::Severity::Error, message, token.span});
    }
}

} // namespace parsing
