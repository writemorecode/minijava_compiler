#include "lexing/Lexer.hpp"

#include <cctype>
#include <string_view>

namespace lexing {

namespace {

bool is_alpha(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) != 0;
}

bool is_digit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch)) != 0;
}

bool is_ident_start(char ch) {
    return is_alpha(ch);
}

bool is_ident_continue(char ch) {
    return is_alpha(ch) || is_digit(ch) || ch == '_';
}

bool starts_with(CharStream *chars, std::string_view text) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (chars->peek(i) != text[i]) {
            return false;
        }
    }
    return true;
}

} // namespace

Lexer::Lexer(std::unique_ptr<CharStream> chars, std::string_view source,
             Diagnostics *diag, LexerOptions opts)
    : chars_(std::move(chars)), source_(source), diag_(diag), opts_(opts) {}

Token Lexer::next() {
    if (!la_.empty()) {
        Token t = la_.front();
        la_.erase(la_.begin());
        return t;
    }
    return lex_one_();
}

Token Lexer::peek(std::size_t n) {
    fill_lookahead_(n);
    return la_[n];
}

bool Lexer::eof() {
    return peek().kind == TokenKind::Eof;
}

Lexer::iterator Lexer::begin() {
    return iterator(this);
}

Lexer::sentinel Lexer::end() {
    return {};
}

void Lexer::reset(std::unique_ptr<CharStream> chars, std::string_view source) {
    chars_ = std::move(chars);
    source_ = source;
    la_.clear();
}

void Lexer::fill_lookahead_(std::size_t n) {
    while (la_.size() <= n) {
        la_.push_back(lex_one_());
    }
}

Token Lexer::lex_one_() {
    if (!chars_) {
        return Token{TokenKind::Eof, {}, {}, {}};
    }

    auto make_token = [&](TokenKind kind, SourceLocation start,
                          SourceLocation end, TokenValue value = {}) {
        std::string_view lexeme{};
        if (!source_.empty() && end.offset >= start.offset &&
            end.offset <= source_.size()) {
            lexeme = source_.substr(start.offset, end.offset - start.offset);
        }
        return Token{kind, lexeme, {start, end}, value};
    };

    auto skip_trivia = [&]() {
        while (!chars_->eof()) {
            const char ch = chars_->peek();
            if (ch == ' ' || ch == '\t' || ch == '\n') {
                chars_->get();
                continue;
            }
            if (ch == '/' && chars_->peek(1) == '/') {
                chars_->get();
                chars_->get();
                while (!chars_->eof() && chars_->peek() != '\n') {
                    chars_->get();
                }
                continue;
            }
            break;
        }
    };

    skip_trivia();

    if (chars_->eof()) {
        const SourceLocation loc = chars_->location();
        return make_token(TokenKind::Eof, loc, loc);
    }

    const SourceLocation start = chars_->location();

    if (starts_with(chars_.get(), "System.out.println")) {
        constexpr std::string_view text = "System.out.println";
        for (char ch : text) {
            (void)ch;
            chars_->get();
        }
        return make_token(TokenKind::KwPrintln, start, chars_->location());
    }

    if (starts_with(chars_.get(), "&&")) {
        chars_->get();
        chars_->get();
        return make_token(TokenKind::AndAnd, start, chars_->location());
    }
    if (starts_with(chars_.get(), "||")) {
        chars_->get();
        chars_->get();
        return make_token(TokenKind::OrOr, start, chars_->location());
    }
    if (starts_with(chars_.get(), "==")) {
        chars_->get();
        chars_->get();
        return make_token(TokenKind::EqEq, start, chars_->location());
    }

    const char ch = chars_->peek();
    switch (ch) {
    case '+':
        chars_->get();
        return make_token(TokenKind::Plus, start, chars_->location());
    case '-':
        chars_->get();
        return make_token(TokenKind::Minus, start, chars_->location());
    case '*':
        chars_->get();
        return make_token(TokenKind::Star, start, chars_->location());
    case '/':
        chars_->get();
        return make_token(TokenKind::Slash, start, chars_->location());
    case '(':
        chars_->get();
        return make_token(TokenKind::LParen, start, chars_->location());
    case ')':
        chars_->get();
        return make_token(TokenKind::RParen, start, chars_->location());
    case '{':
        chars_->get();
        return make_token(TokenKind::LCurly, start, chars_->location());
    case '}':
        chars_->get();
        return make_token(TokenKind::RCurly, start, chars_->location());
    case '[':
        chars_->get();
        return make_token(TokenKind::LSquare, start, chars_->location());
    case ']':
        chars_->get();
        return make_token(TokenKind::RSquare, start, chars_->location());
    case ';':
        chars_->get();
        return make_token(TokenKind::Semi, start, chars_->location());
    case '<':
        chars_->get();
        return make_token(TokenKind::Lt, start, chars_->location());
    case '>':
        chars_->get();
        return make_token(TokenKind::Gt, start, chars_->location());
    case '!':
        chars_->get();
        return make_token(TokenKind::Bang, start, chars_->location());
    case '.':
        chars_->get();
        return make_token(TokenKind::Dot, start, chars_->location());
    case '=':
        chars_->get();
        return make_token(TokenKind::Assign, start, chars_->location());
    case ',':
        chars_->get();
        return make_token(TokenKind::Comma, start, chars_->location());
    default:
        break;
    }

    if (is_digit(ch)) {
        std::int64_t value = 0;
        if (ch == '0') {
            chars_->get();
        } else {
            while (!chars_->eof() && is_digit(chars_->peek())) {
                const char digit = chars_->get();
                value = value * 10 + (digit - '0');
            }
        }
        const SourceLocation end = chars_->location();
        return make_token(TokenKind::IntLiteral, start, end, value);
    }

    if (is_ident_start(ch)) {
        while (!chars_->eof() && is_ident_continue(chars_->peek())) {
            chars_->get();
        }
        const SourceLocation end = chars_->location();
        std::string_view lexeme{};
        if (!source_.empty() && end.offset >= start.offset &&
            end.offset <= source_.size()) {
            lexeme = source_.substr(start.offset, end.offset - start.offset);
        }
        Token token{TokenKind::Identifier, lexeme, {start, end}, {}};
        return token;
    }

    chars_->get();
    return make_token(TokenKind::Invalid, start, chars_->location());
}

Lexer::iterator::iterator(Lexer *lx) : lx_(lx) {
    ensure_loaded_();
}

const Token &Lexer::iterator::operator*() const {
    return *current_;
}

const Token *Lexer::iterator::operator->() const {
    return &(*current_);
}

Lexer::iterator &Lexer::iterator::operator++() {
    ensure_loaded_();
    if (at_end_ || lx_ == nullptr) {
        return *this;
    }

    current_ = lx_->next();
    if (current_->kind == TokenKind::Eof) {
        at_end_ = true;
    }
    return *this;
}

void Lexer::iterator::operator++(int) {
    ++(*this);
}

bool operator==(const Lexer::iterator &it, Lexer::sentinel) {
    return it.at_end_ || it.lx_ == nullptr;
}

bool operator!=(const Lexer::iterator &it, Lexer::sentinel s) {
    return !(it == s);
}

void Lexer::iterator::ensure_loaded_() {
    if (lx_ == nullptr || at_end_) {
        return;
    }
    if (!current_.has_value()) {
        current_ = lx_->next();
        if (current_->kind == TokenKind::Eof) {
            at_end_ = true;
        }
    }
}

} // namespace lexing
