#include "parsing/Parser.hpp"

#include <cassert>
#include <cstddef>
#include <deque>
#include <expected>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "ast/ArithmeticExpressionNode.hpp"
#include "ast/ArrayAccessNode.hpp"
#include "ast/ArrayLengthNode.hpp"
#include "ast/BooleanExpressionNode.hpp"
#include "ast/BooleanNode.hpp"
#include "ast/ClassAllocationNode.hpp"
#include "ast/ClassNode.hpp"
#include "ast/ControlStatementNode.hpp"
#include "ast/IdentifierNode.hpp"
#include "ast/IntegerArrayAllocationNode.hpp"
#include "ast/IntegerNode.hpp"
#include "ast/LogicalExpressionNode.hpp"
#include "ast/MainClassNode.hpp"
#include "ast/MethodBodyNode.hpp"
#include "ast/MethodCallNode.hpp"
#include "ast/MethodCallWithoutArgumentsNode.hpp"
#include "ast/MethodNode.hpp"
#include "ast/MethodParameterNode.hpp"
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/Node.h"
#include "ast/NotNode.hpp"
#include "ast/StatementNode.hpp"
#include "ast/ThisNode.hpp"
#include "ast/VariableNode.hpp"
#include "lexing/Lexer.hpp"
#include "lexing/Token.hpp"
#include "semantic/TypeNode.hpp"

namespace parsing {

namespace {
constexpr int BP_OR = 1;
constexpr int BP_AND = 2;
constexpr int BP_COMP = 3;
constexpr int BP_ADD = 4;
constexpr int BP_MUL = 5;
constexpr int BP_PREFIX_MINUS = 6;
constexpr int BP_POSTFIX = 7;
constexpr int BP_DOT = 8;
constexpr int BP_PREFIX_NOT = 4;

std::string_view token_debug_name(const lexing::Token &token) {
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

std::string_view token_expected_label(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::Eof:
        return "end of file";
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
    case TokenKind::Assign:
        return "ASSIGN";
    default:
        return "token";
    }
}

std::string_view parse_error_expected_label(const ParseError &error) {
    switch (error.kind) {
    case ParseErrorKind::ExpectedToken:
        if (error.expected_token.has_value()) {
            return token_expected_label(error.expected_token.value());
        }
        return "token";
    case ParseErrorKind::ExpectedExpression:
        return "expression";
    case ParseErrorKind::ExpectedStatement:
        return "statement";
    case ParseErrorKind::ExpectedType:
        return "type";
    }
    return "token";
}

struct BindingPower {
    int left = 0;
    int right = 0;
};

std::optional<BindingPower> infix_binding_power(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::OrOr:
        return BindingPower{.left = BP_OR, .right = BP_OR};
    case TokenKind::AndAnd:
        return BindingPower{.left = BP_AND, .right = BP_AND};
    case TokenKind::Lt:
    case TokenKind::Gt:
    case TokenKind::EqEq:
        return BindingPower{.left = BP_COMP, .right = BP_COMP + 1};
    case TokenKind::Plus:
    case TokenKind::Minus:
        return BindingPower{.left = BP_ADD, .right = BP_ADD + 1};
    case TokenKind::Star:
    case TokenKind::Slash:
        return BindingPower{.left = BP_MUL, .right = BP_MUL + 1};
    case TokenKind::LSquare:
        return BindingPower{.left = BP_POSTFIX, .right = BP_POSTFIX + 1};
    case TokenKind::Dot:
        return BindingPower{.left = BP_DOT, .right = BP_DOT + 1};
    default:
        return std::nullopt;
    }
}

bool is_statement_start(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::LCurly:
    case TokenKind::KwIf:
    case TokenKind::KwWhile:
    case TokenKind::KwPrintln:
    case TokenKind::Identifier:
        return true;
    default:
        return false;
    }
}

bool is_type_start(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::KwInt:
    case TokenKind::KwBoolean:
    case TokenKind::Identifier:
        return true;
    default:
        return false;
    }
}
} // namespace

Parser::Parser(lexing::Lexer lexer, lexing::DiagnosticSink *sink)
    : lexer_(std::move(lexer)), sink_(sink) {}

bool Parser::has_errors() const { return error_count_ > 0; }

int Parser::error_count() const { return error_count_; }

const lexing::Token &Parser::peek(std::size_t n) {
    while (buffer_.size() <= n) {
        lexing::Token const token = lexer_.next();
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

Result<lexing::Token> Parser::expect(lexing::TokenKind kind) {
    if (peek().kind == kind) {
        return Result<lexing::Token>(consume());
    }
    const lexing::Token &token = peek();
    ParseError const error{
        .kind = ParseErrorKind::ExpectedToken,
        .expected_token = kind,
        .span = token.span,
    };
    report_error(token, error);
    return std::unexpected(error);
}

void Parser::report_error(const lexing::Token &token, const ParseError &error) {
    if (reported_syntax_error_) {
        return;
    }

    reported_syntax_error_ = true;
    error_count_ += 1;

    std::string err = "syntax error";
    const std::string_view expected_label = parse_error_expected_label(error);
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
        sink_->emit({.severity = lexing::Severity::Error,
                     .message = message,
                     .span = token.span});
    }
}

Result<std::unique_ptr<Node>> Parser::parse_goal() {
    Result<std::unique_ptr<Node>> main_class = parse_main_class();
    if (!main_class.has_value()) {
        return main_class;
    }

    auto root = std::move(main_class.value());

    if (peek().kind == lexing::TokenKind::KwClass) {
        Result<std::unique_ptr<Node>> class_list = parse_class_decl_list();
        if (!class_list.has_value()) {
            return class_list;
        }
        class_list.value()->children.push_front(std::move(root));
        root = std::move(class_list.value());
    }

    Result<lexing::Token> end = expect(lexing::TokenKind::Eof);
    if (!end.has_value()) {
        return std::unexpected(end.error());
    }

    return Result<std::unique_ptr<Node>>(std::move(root));
}

Result<std::unique_ptr<Node>> Parser::parse_main_class() {
    Result<lexing::Token> pub = expect(lexing::TokenKind::KwPublic);
    if (!pub.has_value()) {
        return std::unexpected(pub.error());
    }
    Result<lexing::Token> cls = expect(lexing::TokenKind::KwClass);
    if (!cls.has_value()) {
        return std::unexpected(cls.error());
    }
    Result<std::unique_ptr<Node>> name = parse_identifier();
    if (!name.has_value()) {
        return name;
    }
    Result<lexing::Token> open_class = expect(lexing::TokenKind::LCurly);
    if (!open_class.has_value()) {
        return std::unexpected(open_class.error());
    }
    Result<lexing::Token> pub2 = expect(lexing::TokenKind::KwPublic);
    if (!pub2.has_value()) {
        return std::unexpected(pub2.error());
    }
    Result<lexing::Token> stat = expect(lexing::TokenKind::KwStatic);
    if (!stat.has_value()) {
        return std::unexpected(stat.error());
    }
    Result<lexing::Token> v = expect(lexing::TokenKind::KwVoid);
    if (!v.has_value()) {
        return std::unexpected(v.error());
    }
    Result<lexing::Token> main = expect(lexing::TokenKind::KwMain);
    if (!main.has_value()) {
        return std::unexpected(main.error());
    }
    Result<lexing::Token> lp = expect(lexing::TokenKind::LParen);
    if (!lp.has_value()) {
        return std::unexpected(lp.error());
    }
    Result<lexing::Token> str = expect(lexing::TokenKind::KwString);
    if (!str.has_value()) {
        return std::unexpected(str.error());
    }
    Result<lexing::Token> ls = expect(lexing::TokenKind::LSquare);
    if (!ls.has_value()) {
        return std::unexpected(ls.error());
    }
    Result<lexing::Token> rs = expect(lexing::TokenKind::RSquare);
    if (!rs.has_value()) {
        return std::unexpected(rs.error());
    }
    Result<std::unique_ptr<Node>> arg = parse_identifier();
    if (!arg.has_value()) {
        return arg;
    }
    Result<lexing::Token> rp = expect(lexing::TokenKind::RParen);
    if (!rp.has_value()) {
        return std::unexpected(rp.error());
    }
    Result<lexing::Token> open_main = expect(lexing::TokenKind::LCurly);
    if (!open_main.has_value()) {
        return std::unexpected(open_main.error());
    }
    Result<std::unique_ptr<Node>> stmts = parse_statement_list();
    if (!stmts.has_value()) {
        return stmts;
    }
    Result<lexing::Token> close_main = expect(lexing::TokenKind::RCurly);
    if (!close_main.has_value()) {
        return std::unexpected(close_main.error());
    }
    Result<lexing::Token> close_class = expect(lexing::TokenKind::RCurly);
    if (!close_class.has_value()) {
        return std::unexpected(close_class.error());
    }

    const int line = name.value()->lineno;
    return Result<std::unique_ptr<Node>>(std::make_unique<MainClassNode>(
        std::move(name.value()), std::move(arg.value()),
        std::move(stmts.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_class_decl_list() {
    Result<std::unique_ptr<Node>> first = parse_class_decl();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Class declaration list", "", line);
    list->children.push_back(std::move(first.value()));

    while (peek().kind == lexing::TokenKind::KwClass) {
        Result<std::unique_ptr<Node>> next = parse_class_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_class_decl() {
    Result<lexing::Token> cls = expect(lexing::TokenKind::KwClass);
    if (!cls.has_value()) {
        return std::unexpected(cls.error());
    }
    Result<std::unique_ptr<Node>> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<std::unique_ptr<Node>> body = parse_class_body();
    if (!body.has_value()) {
        return body;
    }

    const int line = id.value()->lineno;
    return Result<std::unique_ptr<Node>>(std::make_unique<ClassNode>(
        std::move(id.value()), std::move(body.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_class_body() {
    Result<lexing::Token> open = expect(lexing::TokenKind::LCurly);
    if (!open.has_value()) {
        return std::unexpected(open.error());
    }

    if (match(lexing::TokenKind::RCurly)) {
        int const line = static_cast<int>(open.value().span.begin.line);
        return Result<std::unique_ptr<Node>>(
            std::make_unique<Node>("Empty class body", "", line));
    }

    std::unique_ptr<Node> var_list;
    std::unique_ptr<Node> method_list;

    if (peek().kind != lexing::TokenKind::KwPublic) {
        Result<std::unique_ptr<Node>> vars = parse_class_var_decl_list();
        if (!vars.has_value()) {
            return vars;
        }
        var_list = std::move(vars.value());
    }

    if (peek().kind == lexing::TokenKind::KwPublic) {
        Result<std::unique_ptr<Node>> methods = parse_method_decl_list();
        if (!methods.has_value()) {
            return methods;
        }
        method_list = std::move(methods.value());
    }

    Result<lexing::Token> close = expect(lexing::TokenKind::RCurly);
    if (!close.has_value()) {
        return std::unexpected(close.error());
    }

    if (var_list != nullptr && method_list != nullptr) {
        int const line = static_cast<int>(close.value().span.begin.line);
        auto body = std::make_unique<Node>("Class body", "", line);
        body->children.push_back(std::move(var_list));
        body->children.push_back(std::move(method_list));
        return Result<std::unique_ptr<Node>>(std::move(body));
    }

    if (var_list != nullptr) {
        return Result<std::unique_ptr<Node>>(std::move(var_list));
    }

    return Result<std::unique_ptr<Node>>(std::move(method_list));
}

Result<std::unique_ptr<Node>> Parser::parse_class_var_decl_list() {
    Result<std::unique_ptr<Node>> first = parse_var_decl();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Variable declaration list", "", line);
    list->children.push_back(std::move(first.value()));

    while (is_type_start(peek().kind)) {
        Result<std::unique_ptr<Node>> next = parse_var_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_method_decl_list() {
    Result<std::unique_ptr<Node>> first = parse_method_decl();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Method declaration list", "", line);
    list->children.push_back(std::move(first.value()));

    while (peek().kind == lexing::TokenKind::KwPublic) {
        Result<std::unique_ptr<Node>> next = parse_method_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_var_decl() {
    Result<std::unique_ptr<Node>> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<std::unique_ptr<Node>> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
    if (!semi.has_value()) {
        return std::unexpected(semi.error());
    }

    const int line = type.value()->lineno;
    return Result<std::unique_ptr<Node>>(std::make_unique<VariableNode>(
        std::move(type.value()), std::move(id.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_method_decl() {
    Result<lexing::Token> pub = expect(lexing::TokenKind::KwPublic);
    if (!pub.has_value()) {
        return std::unexpected(pub.error());
    }
    Result<std::unique_ptr<Node>> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<std::unique_ptr<Node>> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<lexing::Token> open = expect(lexing::TokenKind::LParen);
    if (!open.has_value()) {
        return std::unexpected(open.error());
    }

    if (match(lexing::TokenKind::RParen)) {
        Result<lexing::Token> open_body = expect(lexing::TokenKind::LCurly);
        if (!open_body.has_value()) {
            return std::unexpected(open_body.error());
        }
        Result<std::unique_ptr<Node>> body = parse_method_body();
        if (!body.has_value()) {
            return body;
        }
        Result<lexing::Token> close = expect(lexing::TokenKind::RCurly);
        if (!close.has_value()) {
            return std::unexpected(close.error());
        }
        const int line = type.value()->lineno;
        return Result<std::unique_ptr<Node>>(
            std::make_unique<MethodWithoutParametersNode>(
                std::move(type.value()), std::move(id.value()),
                std::move(body.value()), line));
    }

    Result<std::unique_ptr<Node>> params = parse_method_parameter_list();
    if (!params.has_value()) {
        return params;
    }
    Result<lexing::Token> close_paren = expect(lexing::TokenKind::RParen);
    if (!close_paren.has_value()) {
        return std::unexpected(close_paren.error());
    }
    Result<lexing::Token> open_body = expect(lexing::TokenKind::LCurly);
    if (!open_body.has_value()) {
        return std::unexpected(open_body.error());
    }
    Result<std::unique_ptr<Node>> body = parse_method_body();
    if (!body.has_value()) {
        return body;
    }
    Result<lexing::Token> close = expect(lexing::TokenKind::RCurly);
    if (!close.has_value()) {
        return std::unexpected(close.error());
    }

    const int line = type.value()->lineno;
    return Result<std::unique_ptr<Node>>(std::make_unique<MethodNode>(
        std::move(type.value()), std::move(id.value()),
        std::move(params.value()), std::move(body.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_method_parameter() {
    Result<std::unique_ptr<Node>> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<std::unique_ptr<Node>> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }

    const int line = type.value()->lineno;
    return Result<std::unique_ptr<Node>>(std::make_unique<MethodParameterNode>(
        std::move(type.value()), std::move(id.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_method_parameter_list() {
    Result<std::unique_ptr<Node>> first = parse_method_parameter();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Method parameter list", "", line);
    list->children.push_back(std::move(first.value()));

    while (match(lexing::TokenKind::Comma)) {
        Result<std::unique_ptr<Node>> next = parse_method_parameter();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_method_body() {
    if (peek().kind == lexing::TokenKind::KwReturn) {
        lexing::Token const ret = consume();
        Result<std::unique_ptr<Node>> expr = parse_expression(0);
        if (!expr.has_value()) {
            return expr;
        }
        Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
        if (!semi.has_value()) {
            return std::unexpected(semi.error());
        }
        int const line = static_cast<int>(ret.span.begin.line);
        return Result<std::unique_ptr<Node>>(
            std::make_unique<ReturnOnlyMethodBodyNode>(std::move(expr.value()),
                                                       line));
    }

    Result<std::unique_ptr<Node>> items = parse_method_body_item_list();
    if (!items.has_value()) {
        return items;
    }
    Result<lexing::Token> ret = expect(lexing::TokenKind::KwReturn);
    if (!ret.has_value()) {
        return std::unexpected(ret.error());
    }
    Result<std::unique_ptr<Node>> expr = parse_expression(0);
    if (!expr.has_value()) {
        return expr;
    }
    Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
    if (!semi.has_value()) {
        return std::unexpected(semi.error());
    }
    int const line = static_cast<int>(ret.value().span.begin.line);
    return Result<std::unique_ptr<Node>>(std::make_unique<MethodBodyNode>(
        std::move(items.value()), std::move(expr.value()), line));
}

Result<std::unique_ptr<Node>> Parser::parse_method_body_item() {
    auto looks_like_var_decl = [&]() {
        if (peek().kind == lexing::TokenKind::KwInt ||
            peek().kind == lexing::TokenKind::KwBoolean) {
            return true;
        }
        if (peek().kind != lexing::TokenKind::Identifier) {
            return false;
        }
        const lexing::Token &next = peek(1);
        return next.kind == lexing::TokenKind::Identifier;
    };

    if (looks_like_var_decl()) {
        Result<std::unique_ptr<Node>> var = parse_var_decl();
        if (!var.has_value()) {
            return var;
        }
        int const line = var.value()->lineno;
        auto wrapper = std::make_unique<Node>("Variable declaration", "", line);
        wrapper->children.push_back(std::move(var.value()));
        return Result<std::unique_ptr<Node>>(std::move(wrapper));
    }

    Result<std::unique_ptr<Node>> stmt = parse_statement();
    if (!stmt.has_value()) {
        return stmt;
    }
    int const line = stmt.value()->lineno;
    auto wrapper = std::make_unique<Node>("Statement", "", line);
    wrapper->children.push_back(std::move(stmt.value()));
    return Result<std::unique_ptr<Node>>(std::move(wrapper));
}

Result<std::unique_ptr<Node>> Parser::parse_method_body_item_list() {
    Result<std::unique_ptr<Node>> first = parse_method_body_item();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Method body item list", "", line);
    list->children.push_back(std::move(first.value()));

    while (peek().kind != lexing::TokenKind::KwReturn) {
        Result<std::unique_ptr<Node>> next = parse_method_body_item();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_statement() {
    const lexing::Token &token = peek();
    const int line = static_cast<int>(token.span.begin.line);

    if (match(lexing::TokenKind::LCurly)) {
        if (match(lexing::TokenKind::RCurly)) {
            return Result<std::unique_ptr<Node>>(
                std::make_unique<Node>("Empty statement", line));
        }

        Result<std::unique_ptr<Node>> stmts = parse_statement_list();
        if (!stmts.has_value()) {
            return stmts;
        }
        Result<lexing::Token> close = expect(lexing::TokenKind::RCurly);
        if (!close.has_value()) {
            return std::unexpected(close.error());
        }
        return Result<std::unique_ptr<Node>>(std::move(stmts.value()));
    }

    if (match(lexing::TokenKind::KwIf)) {
        Result<lexing::Token> lp = expect(lexing::TokenKind::LParen);
        if (!lp.has_value()) {
            return std::unexpected(lp.error());
        }
        Result<std::unique_ptr<Node>> cond = parse_expression(0);
        if (!cond.has_value()) {
            return cond;
        }
        Result<lexing::Token> rp = expect(lexing::TokenKind::RParen);
        if (!rp.has_value()) {
            return std::unexpected(rp.error());
        }
        Result<std::unique_ptr<Node>> then_stmt = parse_statement();
        if (!then_stmt.has_value()) {
            return then_stmt;
        }
        if (match(lexing::TokenKind::KwElse)) {
            Result<std::unique_ptr<Node>> else_stmt = parse_statement();
            if (!else_stmt.has_value()) {
                return else_stmt;
            }
            return Result<std::unique_ptr<Node>>(std::make_unique<IfElseNode>(
                std::move(cond.value()), std::move(then_stmt.value()),
                std::move(else_stmt.value()), line));
        }
        return Result<std::unique_ptr<Node>>(std::make_unique<IfNode>(
            std::move(cond.value()), std::move(then_stmt.value()), line));
    }

    if (match(lexing::TokenKind::KwWhile)) {
        Result<lexing::Token> lp = expect(lexing::TokenKind::LParen);
        if (!lp.has_value()) {
            return std::unexpected(lp.error());
        }
        Result<std::unique_ptr<Node>> cond = parse_expression(0);
        if (!cond.has_value()) {
            return cond;
        }
        Result<lexing::Token> rp = expect(lexing::TokenKind::RParen);
        if (!rp.has_value()) {
            return std::unexpected(rp.error());
        }
        Result<std::unique_ptr<Node>> stmt = parse_statement();
        if (!stmt.has_value()) {
            return stmt;
        }
        return Result<std::unique_ptr<Node>>(std::make_unique<WhileNode>(
            std::move(cond.value()), std::move(stmt.value()), line));
    }

    if (match(lexing::TokenKind::KwPrintln)) {
        Result<lexing::Token> lp = expect(lexing::TokenKind::LParen);
        if (!lp.has_value()) {
            return std::unexpected(lp.error());
        }
        Result<std::unique_ptr<Node>> expr = parse_expression(0);
        if (!expr.has_value()) {
            return expr;
        }
        Result<lexing::Token> rp = expect(lexing::TokenKind::RParen);
        if (!rp.has_value()) {
            return std::unexpected(rp.error());
        }
        Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
        if (!semi.has_value()) {
            return std::unexpected(semi.error());
        }
        return Result<std::unique_ptr<Node>>(
            std::make_unique<PrintNode>(std::move(expr.value()), line));
    }

    if (peek().kind == lexing::TokenKind::Identifier) {
        Result<std::unique_ptr<Node>> id = parse_identifier();
        if (!id.has_value()) {
            return id;
        }
        if (match(lexing::TokenKind::Assign)) {
            Result<std::unique_ptr<Node>> expr = parse_expression(0);
            if (!expr.has_value()) {
                return expr;
            }
            Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
            if (!semi.has_value()) {
                return std::unexpected(semi.error());
            }
            return Result<std::unique_ptr<Node>>(std::make_unique<AssignNode>(
                std::move(id.value()), std::move(expr.value()), line));
        }
        if (match(lexing::TokenKind::LSquare)) {
            Result<std::unique_ptr<Node>> index = parse_expression(0);
            if (!index.has_value()) {
                return index;
            }
            Result<lexing::Token> rs = expect(lexing::TokenKind::RSquare);
            if (!rs.has_value()) {
                return std::unexpected(rs.error());
            }
            Result<lexing::Token> assign = expect(lexing::TokenKind::Assign);
            if (!assign.has_value()) {
                return std::unexpected(assign.error());
            }
            Result<std::unique_ptr<Node>> expr = parse_expression(0);
            if (!expr.has_value()) {
                return expr;
            }
            Result<lexing::Token> semi = expect(lexing::TokenKind::Semi);
            if (!semi.has_value()) {
                return std::unexpected(semi.error());
            }
            return Result<std::unique_ptr<Node>>(
                std::make_unique<ArrayAssignNode>(
                    std::move(id.value()), std::move(index.value()),
                    std::move(expr.value()), line));
        }
        const lexing::Token &next = peek();
        ParseError const error{
            .kind = ParseErrorKind::ExpectedToken,
            .expected_token = lexing::TokenKind::Assign,
            .span = next.span,
        };
        report_error(next, error);
        return std::unexpected(error);
    }

    ParseError const error{
        .kind = ParseErrorKind::ExpectedStatement,
        .span = token.span,
    };
    report_error(token, error);
    return std::unexpected(error);
}

Result<std::unique_ptr<Node>> Parser::parse_statement_list() {
    Result<std::unique_ptr<Node>> first = parse_statement();
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Statement list", "", line);
    list->children.push_back(std::move(first.value()));

    while (is_statement_start(peek().kind)) {
        Result<std::unique_ptr<Node>> next = parse_statement();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_expression_list() {
    Result<std::unique_ptr<Node>> first = parse_expression(0);
    if (!first.has_value()) {
        return first;
    }
    int const line = first.value()->lineno;
    auto list = std::make_unique<Node>("Expression list", "", line);
    list->children.push_back(std::move(first.value()));

    while (match(lexing::TokenKind::Comma)) {
        Result<std::unique_ptr<Node>> next = parse_expression(0);
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(std::move(next.value()));
    }

    return Result<std::unique_ptr<Node>>(std::move(list));
}

Result<std::unique_ptr<Node>> Parser::parse_type() {
    const lexing::Token token = peek();
    const int line = static_cast<int>(token.span.begin.line);

    if (match(lexing::TokenKind::KwInt)) {
        if (match(lexing::TokenKind::LSquare)) {
            Result<lexing::Token> rs = expect(lexing::TokenKind::RSquare);
            if (!rs.has_value()) {
                return std::unexpected(rs.error());
            }
            return Result<std::unique_ptr<Node>>(
                std::make_unique<TypeNode>("int[]", line));
        }
        return Result<std::unique_ptr<Node>>(
            std::make_unique<TypeNode>("int", line));
    }

    if (match(lexing::TokenKind::KwBoolean)) {
        return Result<std::unique_ptr<Node>>(
            std::make_unique<TypeNode>("boolean", line));
    }

    if (match(lexing::TokenKind::Identifier)) {
        return Result<std::unique_ptr<Node>>(
            std::make_unique<TypeNode>(std::string(token.lexeme), line));
    }

    ParseError const error{
        .kind = ParseErrorKind::ExpectedType,
        .span = token.span,
    };
    report_error(token, error);
    return std::unexpected(error);
}

Result<std::unique_ptr<Node>> Parser::parse_identifier() {
    Result<lexing::Token> id = expect(lexing::TokenKind::Identifier);
    if (!id.has_value()) {
        return std::unexpected(id.error());
    }
    int const line = static_cast<int>(id.value().span.begin.line);
    return Result<std::unique_ptr<Node>>(
        std::make_unique<IdentifierNode>(std::string(id.value().lexeme), line));
}

Result<std::unique_ptr<Node>> Parser::parse_integer() {
    Result<lexing::Token> lit = expect(lexing::TokenKind::IntLiteral);
    if (!lit.has_value()) {
        return std::unexpected(lit.error());
    }
    int const line = static_cast<int>(lit.value().span.begin.line);
    return Result<std::unique_ptr<Node>>(
        std::make_unique<IntegerNode>(std::string(lit.value().lexeme), line));
}

Result<std::unique_ptr<Node>> Parser::parse_expression(int min_bp) {
    const lexing::Token token = consume();
    const int line = static_cast<int>(token.span.begin.line);
    std::unique_ptr<Node> lhs;

    switch (token.kind) {
    case lexing::TokenKind::IntLiteral: {
        lhs = std::make_unique<IntegerNode>(std::string(token.lexeme), line);
        break;
    }
    case lexing::TokenKind::Identifier: {
        lhs = std::make_unique<IdentifierNode>(std::string(token.lexeme), line);
        break;
    }
    case lexing::TokenKind::KwTrue: {
        lhs = std::make_unique<TrueNode>(line);
        break;
    }
    case lexing::TokenKind::KwFalse: {
        lhs = std::make_unique<FalseNode>(line);
        break;
    }
    case lexing::TokenKind::KwThis: {
        lhs = std::make_unique<ThisNode>(line);
        break;
    }
    case lexing::TokenKind::LParen: {
        Result<std::unique_ptr<Node>> inner = parse_expression(0);
        if (!inner.has_value()) {
            return inner;
        }
        Result<lexing::Token> closing = expect(lexing::TokenKind::RParen);
        if (!closing.has_value()) {
            return std::unexpected(closing.error());
        }
        lhs = std::move(inner.value());
        break;
    }
    case lexing::TokenKind::Bang: {
        Result<std::unique_ptr<Node>> rhs = parse_expression(BP_PREFIX_NOT);
        if (!rhs.has_value()) {
            return rhs;
        }
        lhs = std::make_unique<NotNode>(std::move(rhs.value()), line);
        break;
    }
    case lexing::TokenKind::Minus: {
        Result<std::unique_ptr<Node>> rhs = parse_expression(BP_PREFIX_MINUS);
        if (!rhs.has_value()) {
            return rhs;
        }
        auto zero = std::make_unique<IntegerNode>("0", line);
        lhs = std::make_unique<MinusNode>(std::move(zero),
                                          std::move(rhs.value()), line);
        break;
    }
    case lexing::TokenKind::KwNew: {
        if (match(lexing::TokenKind::KwInt)) {
            Result<lexing::Token> open = expect(lexing::TokenKind::LSquare);
            if (!open.has_value()) {
                return std::unexpected(open.error());
            }
            Result<std::unique_ptr<Node>> length = parse_expression(0);
            if (!length.has_value()) {
                return length;
            }
            Result<lexing::Token> close = expect(lexing::TokenKind::RSquare);
            if (!close.has_value()) {
                return std::unexpected(close.error());
            }
            lhs = std::make_unique<IntegerArrayAllocationNode>(
                std::move(length.value()), line);
            break;
        }

        Result<lexing::Token> id = expect(lexing::TokenKind::Identifier);
        if (!id.has_value()) {
            return std::unexpected(id.error());
        }
        Result<lexing::Token> open = expect(lexing::TokenKind::LParen);
        if (!open.has_value()) {
            return std::unexpected(open.error());
        }
        Result<lexing::Token> close = expect(lexing::TokenKind::RParen);
        if (!close.has_value()) {
            return std::unexpected(close.error());
        }
        auto identifier = std::make_unique<IdentifierNode>(
            std::string(id.value().lexeme), line);
        lhs =
            std::make_unique<ClassAllocationNode>(std::move(identifier), line);
        break;
    }
    default:
        ParseError const error{
            .kind = ParseErrorKind::ExpectedExpression,
            .span = token.span,
        };
        report_error(token, error);
        return std::unexpected(error);
    }

    while (true) {
        const lexing::Token op = peek();
        auto binding = infix_binding_power(op.kind);
        if (!binding.has_value() || binding->left < min_bp) {
            break;
        }

        consume();
        const int op_line = static_cast<int>(op.span.begin.line);

        switch (op.kind) {
        case lexing::TokenKind::Plus: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<PlusNode>(std::move(lhs),
                                             std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::Minus: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<MinusNode>(std::move(lhs),
                                              std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::Star: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<MultiplicationNode>(
                std::move(lhs), std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::Slash: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<DivisionNode>(
                std::move(lhs), std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::OrOr: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<OrNode>(std::move(lhs),
                                           std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::AndAnd: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<AndNode>(std::move(lhs),
                                            std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::Lt: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<LessThanNode>(
                std::move(lhs), std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::Gt: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<GreaterThanNode>(
                std::move(lhs), std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::EqEq: {
            Result<std::unique_ptr<Node>> rhs =
                parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = std::make_unique<EqualToNode>(
                std::move(lhs), std::move(rhs.value()), op_line);
            break;
        }
        case lexing::TokenKind::LSquare: {
            Result<std::unique_ptr<Node>> index = parse_expression(0);
            if (!index.has_value()) {
                return index;
            }
            Result<lexing::Token> close = expect(lexing::TokenKind::RSquare);
            if (!close.has_value()) {
                return std::unexpected(close.error());
            }
            lhs = std::make_unique<ArrayAccessNode>(
                std::move(lhs), std::move(index.value()), op_line);
            break;
        }
        case lexing::TokenKind::Dot: {
            if (match(lexing::TokenKind::KwLength)) {
                lhs =
                    std::make_unique<ArrayLengthNode>(std::move(lhs), op_line);
                break;
            }

            Result<lexing::Token> id = expect(lexing::TokenKind::Identifier);
            if (!id.has_value()) {
                return std::unexpected(id.error());
            }
            Result<lexing::Token> open = expect(lexing::TokenKind::LParen);
            if (!open.has_value()) {
                return std::unexpected(open.error());
            }

            if (match(lexing::TokenKind::RParen)) {
                auto identifier = std::make_unique<IdentifierNode>(
                    std::string(id.value().lexeme), op_line);
                lhs = std::make_unique<MethodCallWithoutArgumentsNode>(
                    std::move(lhs), std::move(identifier), op_line);
                break;
            }

            Result<std::unique_ptr<Node>> expr_list = parse_expression_list();
            if (!expr_list.has_value()) {
                return expr_list;
            }
            Result<lexing::Token> close = expect(lexing::TokenKind::RParen);
            if (!close.has_value()) {
                return std::unexpected(close.error());
            }

            auto identifier = std::make_unique<IdentifierNode>(
                std::string(id.value().lexeme), op_line);
            lhs = std::make_unique<MethodCallNode>(
                std::move(lhs), std::move(identifier),
                std::move(expr_list.value()), op_line);
            break;
        }
        default:
            ParseError const error{
                .kind = ParseErrorKind::ExpectedExpression,
                .span = op.span,
            };
            report_error(op, error);
            return std::unexpected(error);
        }
    }

    return Result<std::unique_ptr<Node>>(std::move(lhs));
}

} // namespace parsing
