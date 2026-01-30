#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <vector>

#include "lexing/CharStream.hpp"
#include "lexing/Token.hpp"

namespace lexing {

struct LexerOptions {
    bool emit_trivia = false;
};

class Lexer {
  public:
    using Diagnostics = DiagnosticSink;

    Lexer(std::unique_ptr<CharStream> chars, Diagnostics *diag = nullptr,
          LexerOptions opts = {});

    Token next();
    Token peek(std::size_t n = 0);
    bool eof();

    class iterator;
    struct sentinel {};

    iterator begin();
    sentinel end();

    void reset(std::unique_ptr<CharStream> chars);
    const LexerOptions &options() const { return opts_; }

  private:
    Token lex_one_();
    void fill_lookahead_(std::size_t n);

    std::unique_ptr<CharStream> chars_;
    Diagnostics *diag_ = nullptr;
    LexerOptions opts_{};
    std::vector<Token> la_{};
};

class Lexer::iterator {
  public:
    using value_type = Token;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    iterator() = default;
    explicit iterator(Lexer *lx);

    const Token &operator*() const;
    const Token *operator->() const;

    iterator &operator++();
    void operator++(int);

    friend bool operator==(const iterator &it, sentinel);
    friend bool operator!=(const iterator &it, sentinel);

  private:
    void ensure_loaded_();

    Lexer *lx_ = nullptr;
    std::optional<Token> current_;
    bool at_end_ = false;
};

} // namespace lexing
