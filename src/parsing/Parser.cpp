#include "parsing/Parser.hpp"

#include <cassert>
#include <deque>
#include <optional>
#include <string>

#include "ast/ArithmeticExpressionNode.hpp"
#include "ast/ArrayAccessNode.hpp"
#include "ast/ArrayLengthNode.hpp"
#include "ast/BooleanExpressionNode.hpp"
#include "ast/BooleanNode.hpp"
#include "ast/ClassAllocationNode.hpp"
#include "ast/ClassNode.hpp"
#include "ast/ControlStatementNode.hpp"
#include "ast/IdentifierNode.hpp"
#include "ast/MainClassNode.hpp"
#include "ast/MethodBodyNode.hpp"
#include "ast/MethodCallNode.hpp"
#include "ast/MethodCallWithoutArgumentsNode.hpp"
#include "ast/MethodNode.hpp"
#include "ast/MethodParameterNode.hpp"
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/IntegerArrayAllocationNode.hpp"
#include "ast/IntegerNode.hpp"
#include "ast/LogicalExpressionNode.hpp"
#include "ast/NotNode.hpp"
#include "ast/StatementNode.hpp"
#include "ast/ThisNode.hpp"
#include "ast/VariableNode.hpp"
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

struct BindingPower {
    int left = 0;
    int right = 0;
};

std::optional<BindingPower> infix_binding_power(lexing::TokenKind kind) {
    using lexing::TokenKind;
    switch (kind) {
    case TokenKind::OrOr:
        return BindingPower{BP_OR, BP_OR};
    case TokenKind::AndAnd:
        return BindingPower{BP_AND, BP_AND};
    case TokenKind::Lt:
    case TokenKind::Gt:
    case TokenKind::EqEq:
        return BindingPower{BP_COMP, BP_COMP + 1};
    case TokenKind::Plus:
    case TokenKind::Minus:
        return BindingPower{BP_ADD, BP_ADD + 1};
    case TokenKind::Star:
    case TokenKind::Slash:
        return BindingPower{BP_MUL, BP_MUL + 1};
    case TokenKind::LSquare:
        return BindingPower{BP_POSTFIX, BP_POSTFIX + 1};
    case TokenKind::Dot:
        return BindingPower{BP_DOT, BP_DOT + 1};
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

Result<Node *> Parser::parse_goal() {
    Result<Node *> main_class = parse_main_class();
    if (!main_class.has_value()) {
        return main_class;
    }

    Node *root = main_class.value();

    if (peek().kind == lexing::TokenKind::KwClass) {
        Result<Node *> class_list = parse_class_decl_list();
        if (!class_list.has_value()) {
            return class_list;
        }
        class_list.value()->children.push_front(root);
        root = class_list.value();
    }

    Result<lexing::Token> end = expect(lexing::TokenKind::Eof, "end of file");
    if (!end.has_value()) {
        return Result<Node *>::err(end.error());
    }

    return Result<Node *>::ok(root);
}

Result<Node *> Parser::parse_main_class() {
    Result<lexing::Token> pub =
        expect(lexing::TokenKind::KwPublic, "PUBLIC");
    if (!pub.has_value()) {
        return Result<Node *>::err(pub.error());
    }
    Result<lexing::Token> cls =
        expect(lexing::TokenKind::KwClass, "CLASS");
    if (!cls.has_value()) {
        return Result<Node *>::err(cls.error());
    }
    Result<Node *> name = parse_identifier();
    if (!name.has_value()) {
        return name;
    }
    Result<lexing::Token> open_class =
        expect(lexing::TokenKind::LCurly, "L_CURLY");
    if (!open_class.has_value()) {
        return Result<Node *>::err(open_class.error());
    }
    Result<lexing::Token> pub2 =
        expect(lexing::TokenKind::KwPublic, "PUBLIC");
    if (!pub2.has_value()) {
        return Result<Node *>::err(pub2.error());
    }
    Result<lexing::Token> stat =
        expect(lexing::TokenKind::KwStatic, "STATIC");
    if (!stat.has_value()) {
        return Result<Node *>::err(stat.error());
    }
    Result<lexing::Token> v =
        expect(lexing::TokenKind::KwVoid, "VOID");
    if (!v.has_value()) {
        return Result<Node *>::err(v.error());
    }
    Result<lexing::Token> main =
        expect(lexing::TokenKind::KwMain, "MAIN");
    if (!main.has_value()) {
        return Result<Node *>::err(main.error());
    }
    Result<lexing::Token> lp =
        expect(lexing::TokenKind::LParen, "L_PAREN");
    if (!lp.has_value()) {
        return Result<Node *>::err(lp.error());
    }
    Result<lexing::Token> str =
        expect(lexing::TokenKind::KwString, "STRING");
    if (!str.has_value()) {
        return Result<Node *>::err(str.error());
    }
    Result<lexing::Token> ls =
        expect(lexing::TokenKind::LSquare, "L_SQUARE");
    if (!ls.has_value()) {
        return Result<Node *>::err(ls.error());
    }
    Result<lexing::Token> rs =
        expect(lexing::TokenKind::RSquare, "R_SQUARE");
    if (!rs.has_value()) {
        return Result<Node *>::err(rs.error());
    }
    Result<Node *> arg = parse_identifier();
    if (!arg.has_value()) {
        return arg;
    }
    Result<lexing::Token> rp =
        expect(lexing::TokenKind::RParen, "R_PAREN");
    if (!rp.has_value()) {
        return Result<Node *>::err(rp.error());
    }
    Result<lexing::Token> open_main =
        expect(lexing::TokenKind::LCurly, "L_CURLY");
    if (!open_main.has_value()) {
        return Result<Node *>::err(open_main.error());
    }
    Result<Node *> stmts = parse_statement_list();
    if (!stmts.has_value()) {
        return stmts;
    }
    Result<lexing::Token> close_main =
        expect(lexing::TokenKind::RCurly, "R_CURLY");
    if (!close_main.has_value()) {
        return Result<Node *>::err(close_main.error());
    }
    Result<lexing::Token> close_class =
        expect(lexing::TokenKind::RCurly, "R_CURLY");
    if (!close_class.has_value()) {
        return Result<Node *>::err(close_class.error());
    }

    return Result<Node *>::ok(new MainClassNode(
        name.value(), arg.value(), stmts.value(), name.value()->lineno));
}

Result<Node *> Parser::parse_class_decl_list() {
    Result<Node *> first = parse_class_decl();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Class declaration list", "", line);
    list->children.push_back(first.value());

    while (peek().kind == lexing::TokenKind::KwClass) {
        Result<Node *> next = parse_class_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_class_decl() {
    Result<lexing::Token> cls =
        expect(lexing::TokenKind::KwClass, "CLASS");
    if (!cls.has_value()) {
        return Result<Node *>::err(cls.error());
    }
    Result<Node *> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<Node *> body = parse_class_body();
    if (!body.has_value()) {
        return body;
    }

    return Result<Node *>::ok(
        new ClassNode(id.value(), body.value(), id.value()->lineno));
}

Result<Node *> Parser::parse_class_body() {
    Result<lexing::Token> open =
        expect(lexing::TokenKind::LCurly, "L_CURLY");
    if (!open.has_value()) {
        return Result<Node *>::err(open.error());
    }

    if (match(lexing::TokenKind::RCurly)) {
        int line = static_cast<int>(open.value().span.begin.line);
        return Result<Node *>::ok(new Node("Empty class body", "", line));
    }

    Node *var_list = nullptr;
    Node *method_list = nullptr;

    if (peek().kind != lexing::TokenKind::KwPublic) {
        Result<Node *> vars = parse_class_var_decl_list();
        if (!vars.has_value()) {
            return vars;
        }
        var_list = vars.value();
    }

    if (peek().kind == lexing::TokenKind::KwPublic) {
        Result<Node *> methods = parse_method_decl_list();
        if (!methods.has_value()) {
            return methods;
        }
        method_list = methods.value();
    }

    Result<lexing::Token> close =
        expect(lexing::TokenKind::RCurly, "R_CURLY");
    if (!close.has_value()) {
        return Result<Node *>::err(close.error());
    }

    if (var_list != nullptr && method_list != nullptr) {
        int line = static_cast<int>(close.value().span.begin.line);
        Node *body = new Node("Class body", "", line);
        body->children.push_back(var_list);
        body->children.push_back(method_list);
        return Result<Node *>::ok(body);
    }

    if (var_list != nullptr) {
        return Result<Node *>::ok(var_list);
    }

    return Result<Node *>::ok(method_list);
}

Result<Node *> Parser::parse_class_var_decl_list() {
    Result<Node *> first = parse_var_decl();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Variable declaration list", "", line);
    list->children.push_back(first.value());

    while (is_type_start(peek().kind)) {
        Result<Node *> next = parse_var_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_method_decl_list() {
    Result<Node *> first = parse_method_decl();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Method declaration list", "", line);
    list->children.push_back(first.value());

    while (peek().kind == lexing::TokenKind::KwPublic) {
        Result<Node *> next = parse_method_decl();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_var_decl() {
    Result<Node *> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<Node *> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<lexing::Token> semi =
        expect(lexing::TokenKind::Semi, "SEMI");
    if (!semi.has_value()) {
        return Result<Node *>::err(semi.error());
    }

    return Result<Node *>::ok(
        new VariableNode(type.value(), id.value(), type.value()->lineno));
}

Result<Node *> Parser::parse_method_decl() {
    Result<lexing::Token> pub =
        expect(lexing::TokenKind::KwPublic, "PUBLIC");
    if (!pub.has_value()) {
        return Result<Node *>::err(pub.error());
    }
    Result<Node *> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<Node *> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }
    Result<lexing::Token> open =
        expect(lexing::TokenKind::LParen, "L_PAREN");
    if (!open.has_value()) {
        return Result<Node *>::err(open.error());
    }

    if (match(lexing::TokenKind::RParen)) {
        Result<lexing::Token> open_body =
            expect(lexing::TokenKind::LCurly, "L_CURLY");
        if (!open_body.has_value()) {
            return Result<Node *>::err(open_body.error());
        }
        Result<Node *> body = parse_method_body();
        if (!body.has_value()) {
            return body;
        }
        Result<lexing::Token> close =
            expect(lexing::TokenKind::RCurly, "R_CURLY");
        if (!close.has_value()) {
            return Result<Node *>::err(close.error());
        }
        return Result<Node *>::ok(new MethodWithoutParametersNode(
            type.value(), id.value(), body.value(), type.value()->lineno));
    }

    Result<Node *> params = parse_method_parameter_list();
    if (!params.has_value()) {
        return params;
    }
    Result<lexing::Token> close_paren =
        expect(lexing::TokenKind::RParen, "R_PAREN");
    if (!close_paren.has_value()) {
        return Result<Node *>::err(close_paren.error());
    }
    Result<lexing::Token> open_body =
        expect(lexing::TokenKind::LCurly, "L_CURLY");
    if (!open_body.has_value()) {
        return Result<Node *>::err(open_body.error());
    }
    Result<Node *> body = parse_method_body();
    if (!body.has_value()) {
        return body;
    }
    Result<lexing::Token> close =
        expect(lexing::TokenKind::RCurly, "R_CURLY");
    if (!close.has_value()) {
        return Result<Node *>::err(close.error());
    }

    return Result<Node *>::ok(new MethodNode(type.value(), id.value(),
                                             params.value(), body.value(),
                                             type.value()->lineno));
}

Result<Node *> Parser::parse_method_parameter() {
    Result<Node *> type = parse_type();
    if (!type.has_value()) {
        return type;
    }
    Result<Node *> id = parse_identifier();
    if (!id.has_value()) {
        return id;
    }

    return Result<Node *>::ok(
        new MethodParameterNode(type.value(), id.value(), type.value()->lineno));
}

Result<Node *> Parser::parse_method_parameter_list() {
    Result<Node *> first = parse_method_parameter();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Method parameter list", "", line);
    list->children.push_back(first.value());

    while (match(lexing::TokenKind::Comma)) {
        Result<Node *> next = parse_method_parameter();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_method_body() {
    if (match(lexing::TokenKind::KwReturn)) {
        Result<Node *> expr = parse_expression(0);
        if (!expr.has_value()) {
            return expr;
        }
        Result<lexing::Token> semi =
            expect(lexing::TokenKind::Semi, "SEMI");
        if (!semi.has_value()) {
            return Result<Node *>::err(semi.error());
        }
        int line = static_cast<int>(semi.value().span.begin.line);
        return Result<Node *>::ok(new ReturnOnlyMethodBodyNode(expr.value(),
                                                               line));
    }

    Result<Node *> items = parse_method_body_item_list();
    if (!items.has_value()) {
        return items;
    }
    Result<lexing::Token> ret =
        expect(lexing::TokenKind::KwReturn, "RETURN");
    if (!ret.has_value()) {
        return Result<Node *>::err(ret.error());
    }
    Result<Node *> expr = parse_expression(0);
    if (!expr.has_value()) {
        return expr;
    }
    Result<lexing::Token> semi = expect(lexing::TokenKind::Semi, "SEMI");
    if (!semi.has_value()) {
        return Result<Node *>::err(semi.error());
    }
    int line = static_cast<int>(ret.value().span.begin.line);
    return Result<Node *>::ok(
        new MethodBodyNode(items.value(), expr.value(), line));
}

Result<Node *> Parser::parse_method_body_item() {
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
        Result<Node *> var = parse_var_decl();
        if (!var.has_value()) {
            return var;
        }
        int line = var.value()->lineno;
        Node *wrapper = new Node("Variable declaration", "", line);
        wrapper->children.push_back(var.value());
        return Result<Node *>::ok(wrapper);
    }

    Result<Node *> stmt = parse_statement();
    if (!stmt.has_value()) {
        return stmt;
    }
    int line = stmt.value()->lineno;
    Node *wrapper = new Node("Statement", "", line);
    wrapper->children.push_back(stmt.value());
    return Result<Node *>::ok(wrapper);
}

Result<Node *> Parser::parse_method_body_item_list() {
    Result<Node *> first = parse_method_body_item();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Method body item list", "", line);
    list->children.push_back(first.value());

    while (peek().kind != lexing::TokenKind::KwReturn) {
        Result<Node *> next = parse_method_body_item();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_statement() {
    const lexing::Token &token = peek();
    const int line = static_cast<int>(token.span.begin.line);

    if (match(lexing::TokenKind::LCurly)) {
        if (match(lexing::TokenKind::RCurly)) {
            return Result<Node *>::ok(new Node("Empty statement", line));
        }

        Result<Node *> stmts = parse_statement_list();
        if (!stmts.has_value()) {
            return stmts;
        }
        Result<lexing::Token> close =
            expect(lexing::TokenKind::RCurly, "R_CURLY");
        if (!close.has_value()) {
            return Result<Node *>::err(close.error());
        }
        return Result<Node *>::ok(stmts.value());
    }

    if (match(lexing::TokenKind::KwIf)) {
        Result<lexing::Token> lp =
            expect(lexing::TokenKind::LParen, "L_PAREN");
        if (!lp.has_value()) {
            return Result<Node *>::err(lp.error());
        }
        Result<Node *> cond = parse_expression(0);
        if (!cond.has_value()) {
            return cond;
        }
        Result<lexing::Token> rp =
            expect(lexing::TokenKind::RParen, "R_PAREN");
        if (!rp.has_value()) {
            return Result<Node *>::err(rp.error());
        }
        Result<Node *> then_stmt = parse_statement();
        if (!then_stmt.has_value()) {
            return then_stmt;
        }
        if (match(lexing::TokenKind::KwElse)) {
            Result<Node *> else_stmt = parse_statement();
            if (!else_stmt.has_value()) {
                return else_stmt;
            }
            return Result<Node *>::ok(
                new IfElseNode(cond.value(), then_stmt.value(),
                               else_stmt.value(), line));
        }
        return Result<Node *>::ok(
            new IfNode(cond.value(), then_stmt.value(), line));
    }

    if (match(lexing::TokenKind::KwWhile)) {
        Result<lexing::Token> lp =
            expect(lexing::TokenKind::LParen, "L_PAREN");
        if (!lp.has_value()) {
            return Result<Node *>::err(lp.error());
        }
        Result<Node *> cond = parse_expression(0);
        if (!cond.has_value()) {
            return cond;
        }
        Result<lexing::Token> rp =
            expect(lexing::TokenKind::RParen, "R_PAREN");
        if (!rp.has_value()) {
            return Result<Node *>::err(rp.error());
        }
        Result<Node *> stmt = parse_statement();
        if (!stmt.has_value()) {
            return stmt;
        }
        return Result<Node *>::ok(
            new WhileNode(cond.value(), stmt.value(), line));
    }

    if (match(lexing::TokenKind::KwPrintln)) {
        Result<lexing::Token> lp =
            expect(lexing::TokenKind::LParen, "L_PAREN");
        if (!lp.has_value()) {
            return Result<Node *>::err(lp.error());
        }
        Result<Node *> expr = parse_expression(0);
        if (!expr.has_value()) {
            return expr;
        }
        Result<lexing::Token> rp =
            expect(lexing::TokenKind::RParen, "R_PAREN");
        if (!rp.has_value()) {
            return Result<Node *>::err(rp.error());
        }
        Result<lexing::Token> semi =
            expect(lexing::TokenKind::Semi, "SEMI");
        if (!semi.has_value()) {
            return Result<Node *>::err(semi.error());
        }
        return Result<Node *>::ok(new PrintNode(expr.value(), line));
    }

    if (peek().kind == lexing::TokenKind::Identifier) {
        Result<Node *> id = parse_identifier();
        if (!id.has_value()) {
            return id;
        }
        if (match(lexing::TokenKind::Assign)) {
            Result<Node *> expr = parse_expression(0);
            if (!expr.has_value()) {
                return expr;
            }
            Result<lexing::Token> semi =
                expect(lexing::TokenKind::Semi, "SEMI");
            if (!semi.has_value()) {
                return Result<Node *>::err(semi.error());
            }
            return Result<Node *>::ok(
                new AssignNode(id.value(), expr.value(), line));
        }
        if (match(lexing::TokenKind::LSquare)) {
            Result<Node *> index = parse_expression(0);
            if (!index.has_value()) {
                return index;
            }
            Result<lexing::Token> rs =
                expect(lexing::TokenKind::RSquare, "R_SQUARE");
            if (!rs.has_value()) {
                return Result<Node *>::err(rs.error());
            }
            Result<lexing::Token> assign =
                expect(lexing::TokenKind::Assign, "ASSIGN");
            if (!assign.has_value()) {
                return Result<Node *>::err(assign.error());
            }
            Result<Node *> expr = parse_expression(0);
            if (!expr.has_value()) {
                return expr;
            }
            Result<lexing::Token> semi =
                expect(lexing::TokenKind::Semi, "SEMI");
            if (!semi.has_value()) {
                return Result<Node *>::err(semi.error());
            }
            return Result<Node *>::ok(new ArrayAssignNode(
                id.value(), index.value(), expr.value(), line));
        }
        report_error(peek(), "ASSIGN");
        return Result<Node *>::err(ParseError{"ASSIGN", peek().span});
    }

    report_error(token, "statement");
    return Result<Node *>::err(ParseError{"statement", token.span});
}

Result<Node *> Parser::parse_statement_list() {
    Result<Node *> first = parse_statement();
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Statement list", "", line);
    list->children.push_back(first.value());

    while (is_statement_start(peek().kind)) {
        Result<Node *> next = parse_statement();
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_expression_list() {
    Result<Node *> first = parse_expression(0);
    if (!first.has_value()) {
        return first;
    }
    int line = first.value()->lineno;
    Node *list = new Node("Expression list", "", line);
    list->children.push_back(first.value());

    while (match(lexing::TokenKind::Comma)) {
        Result<Node *> next = parse_expression(0);
        if (!next.has_value()) {
            return next;
        }
        list->children.push_back(next.value());
    }

    return Result<Node *>::ok(list);
}

Result<Node *> Parser::parse_type() {
    const lexing::Token token = peek();
    const int line = static_cast<int>(token.span.begin.line);

    if (match(lexing::TokenKind::KwInt)) {
        if (match(lexing::TokenKind::LSquare)) {
            Result<lexing::Token> rs =
                expect(lexing::TokenKind::RSquare, "R_SQUARE");
            if (!rs.has_value()) {
                return Result<Node *>::err(rs.error());
            }
            return Result<Node *>::ok(new TypeNode("int[]", line));
        }
        return Result<Node *>::ok(new TypeNode("int", line));
    }

    if (match(lexing::TokenKind::KwBoolean)) {
        return Result<Node *>::ok(new TypeNode("boolean", line));
    }

    if (match(lexing::TokenKind::Identifier)) {
        return Result<Node *>::ok(new TypeNode(std::string(token.lexeme), line));
    }

    report_error(token, "type");
    return Result<Node *>::err(ParseError{"type", token.span});
}

Result<Node *> Parser::parse_identifier() {
    Result<lexing::Token> id =
        expect(lexing::TokenKind::Identifier, "ID");
    if (!id.has_value()) {
        return Result<Node *>::err(id.error());
    }
    int line = static_cast<int>(id.value().span.begin.line);
    return Result<Node *>::ok(
        new IdentifierNode(std::string(id.value().lexeme), line));
}

Result<Node *> Parser::parse_integer() {
    Result<lexing::Token> lit =
        expect(lexing::TokenKind::IntLiteral, "INT_LITERAL");
    if (!lit.has_value()) {
        return Result<Node *>::err(lit.error());
    }
    int line = static_cast<int>(lit.value().span.begin.line);
    return Result<Node *>::ok(
        new IntegerNode(std::string(lit.value().lexeme), line));
}

Result<Node *> Parser::parse_expression(int min_bp) {
    const lexing::Token token = consume();
    const int line = static_cast<int>(token.span.begin.line);
    Node *lhs = nullptr;

    switch (token.kind) {
    case lexing::TokenKind::IntLiteral: {
        lhs = new IntegerNode(std::string(token.lexeme), line);
        break;
    }
    case lexing::TokenKind::Identifier: {
        lhs = new IdentifierNode(std::string(token.lexeme), line);
        break;
    }
    case lexing::TokenKind::KwTrue: {
        lhs = new TrueNode(line);
        break;
    }
    case lexing::TokenKind::KwFalse: {
        lhs = new FalseNode(line);
        break;
    }
    case lexing::TokenKind::KwThis: {
        lhs = new ThisNode(line);
        break;
    }
    case lexing::TokenKind::LParen: {
        Result<Node *> inner = parse_expression(0);
        if (!inner.has_value()) {
            return inner;
        }
        Result<lexing::Token> closing =
            expect(lexing::TokenKind::RParen, "R_PAREN");
        if (!closing.has_value()) {
            return Result<Node *>::err(closing.error());
        }
        lhs = inner.value();
        break;
    }
    case lexing::TokenKind::Bang: {
        Result<Node *> rhs = parse_expression(BP_PREFIX_NOT);
        if (!rhs.has_value()) {
            return rhs;
        }
        lhs = new NotNode(rhs.value(), line);
        break;
    }
    case lexing::TokenKind::Minus: {
        Result<Node *> rhs = parse_expression(BP_PREFIX_MINUS);
        if (!rhs.has_value()) {
            return rhs;
        }
        Node *zero = new IntegerNode("0", line);
        lhs = new MinusNode(zero, rhs.value(), line);
        break;
    }
    case lexing::TokenKind::KwNew: {
        if (match(lexing::TokenKind::KwInt)) {
            Result<lexing::Token> open =
                expect(lexing::TokenKind::LSquare, "L_SQUARE");
            if (!open.has_value()) {
                return Result<Node *>::err(open.error());
            }
            Result<Node *> length = parse_expression(0);
            if (!length.has_value()) {
                return length;
            }
            Result<lexing::Token> close =
                expect(lexing::TokenKind::RSquare, "R_SQUARE");
            if (!close.has_value()) {
                return Result<Node *>::err(close.error());
            }
            lhs = new IntegerArrayAllocationNode(length.value(), line);
            break;
        }

        Result<lexing::Token> id =
            expect(lexing::TokenKind::Identifier, "ID");
        if (!id.has_value()) {
            return Result<Node *>::err(id.error());
        }
        Result<lexing::Token> open =
            expect(lexing::TokenKind::LParen, "L_PAREN");
        if (!open.has_value()) {
            return Result<Node *>::err(open.error());
        }
        Result<lexing::Token> close =
            expect(lexing::TokenKind::RParen, "R_PAREN");
        if (!close.has_value()) {
            return Result<Node *>::err(close.error());
        }
        Node *identifier =
            new IdentifierNode(std::string(id.value().lexeme), line);
        lhs = new ClassAllocationNode(identifier, line);
        break;
    }
    default:
        report_error(token, "expression");
        return Result<Node *>::err(ParseError{"expression", token.span});
    }

    while (true) {
        const lexing::Token &op = peek();
        auto binding = infix_binding_power(op.kind);
        if (!binding.has_value() || binding->left < min_bp) {
            break;
        }

        consume();
        const int op_line = static_cast<int>(op.span.begin.line);

        switch (op.kind) {
        case lexing::TokenKind::Plus: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new PlusNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::Minus: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new MinusNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::Star: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new MultiplicationNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::Slash: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new DivisionNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::OrOr: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new OrNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::AndAnd: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new AndNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::Lt: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new LessThanNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::Gt: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new GreaterThanNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::EqEq: {
            Result<Node *> rhs = parse_expression(binding->right);
            if (!rhs.has_value()) {
                return rhs;
            }
            lhs = new EqualToNode(lhs, rhs.value(), op_line);
            break;
        }
        case lexing::TokenKind::LSquare: {
            Result<Node *> index = parse_expression(0);
            if (!index.has_value()) {
                return index;
            }
            Result<lexing::Token> close =
                expect(lexing::TokenKind::RSquare, "R_SQUARE");
            if (!close.has_value()) {
                return Result<Node *>::err(close.error());
            }
            lhs = new ArrayAccessNode(lhs, index.value(), op_line);
            break;
        }
        case lexing::TokenKind::Dot: {
            if (match(lexing::TokenKind::KwLength)) {
                lhs = new ArrayLengthNode(lhs, op_line);
                break;
            }

            Result<lexing::Token> id =
                expect(lexing::TokenKind::Identifier, "ID");
            if (!id.has_value()) {
                return Result<Node *>::err(id.error());
            }
            Result<lexing::Token> open =
                expect(lexing::TokenKind::LParen, "L_PAREN");
            if (!open.has_value()) {
                return Result<Node *>::err(open.error());
            }

            if (match(lexing::TokenKind::RParen)) {
                Node *identifier =
                    new IdentifierNode(std::string(id.value().lexeme), op_line);
                lhs = new MethodCallWithoutArgumentsNode(lhs, identifier,
                                                         op_line);
                break;
            }

            Result<Node *> expr_list = parse_expression_list();
            if (!expr_list.has_value()) {
                return expr_list;
            }
            Result<lexing::Token> close =
                expect(lexing::TokenKind::RParen, "R_PAREN");
            if (!close.has_value()) {
                return Result<Node *>::err(close.error());
            }

            Node *identifier =
                new IdentifierNode(std::string(id.value().lexeme), op_line);
            lhs = new MethodCallNode(lhs, identifier, expr_list.value(),
                                     op_line);
            break;
        }
        default:
            report_error(op, "expression");
            return Result<Node *>::err(ParseError{"expression", op.span});
        }
    }

    return Result<Node *>::ok(lhs);
}

} // namespace parsing
