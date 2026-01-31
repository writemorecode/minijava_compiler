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
#include "ast/IdentifierNode.hpp"
#include "ast/IntegerArrayAllocationNode.hpp"
#include "ast/IntegerNode.hpp"
#include "ast/LogicalExpressionNode.hpp"
#include "ast/MethodCallNode.hpp"
#include "ast/MethodCallWithoutArgumentsNode.hpp"
#include "ast/NotNode.hpp"
#include "ast/ThisNode.hpp"

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
