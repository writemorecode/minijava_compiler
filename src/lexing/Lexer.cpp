#include "lexing/Lexer.hpp"

namespace lexing {

Lexer::Lexer(std::unique_ptr<CharStream> chars, Diagnostics *diag,
             LexerOptions opts)
    : chars_(std::move(chars)), diag_(diag), opts_(opts) {}

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

void Lexer::reset(std::unique_ptr<CharStream> chars) {
    chars_ = std::move(chars);
    la_.clear();
}

void Lexer::fill_lookahead_(std::size_t n) {
    while (la_.size() <= n) {
        la_.push_back(lex_one_());
    }
}

Token Lexer::lex_one_() {
    return Token{TokenKind::Eof, {}, {}, {}};
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
