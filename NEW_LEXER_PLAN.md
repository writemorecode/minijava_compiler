# New Lexer Implementation Plan (Detailed)

## Scope
This plan covers implementing the new pure C++ lexer only (streaming token iterator), aligned with `LEXER_PARSER_REFACTOR_SPEC.md` and the existing Flex rules in `src/lexing/lexer.flex`. Parser work is out of scope except for API integration points noted where necessary.

## Key Constraints
- C++20, no exceptions.
- Tokens store `std::string_view` into the original program source buffer; the buffer must outlive all tokens.
- Error messages must match current output exactly (Flex/Bison behavior).
- Lexer is streaming (peek/next + input iterator).
- Add CLI flag for lex-only mode (prints token names).

## Plan

### 1) Inventory current lexing behavior
- Enumerate token names and keywords from `src/lexing/lexer.flex`.
- Confirm longest-match rules and ordering:
  - `System.out.println` recognized as a single token.
  - Multi-char ops (`&&`, `||`, `==`) before single-char ops.
  - Identifiers vs keywords.
  - Comments (`//`) and whitespace skipped.
- Capture exact lexical error message format:
  - `Line %d: lexical ('%s' symbol is not recognized by the grammar)`

### 2) Define token model and diagnostics
- Create `src/lexing/Token.hpp` with:
  - `SourceLocation`, `SourceSpan`, `Severity`, `Diagnostic`, `DiagnosticSink`.
  - `TokenKind` enum (MiniJava-specific).
  - `Token` struct with `kind`, `lexeme`, `span`, `value` (int64_t or monostate).
- Confirm token kinds map one-to-one with current Bison terminals.

### 3) Source buffer ownership strategy
- Add a small `SourceBuffer` type (e.g., in `src/lexing/SourceBuffer.hpp`) that:
  - Owns the full file contents as `std::string`.
  - Provides `std::string_view view()` and `const char* data()`.
- Update main flow (later) to read the full file into `SourceBuffer` and pass a `StringViewStream` into the lexer.
- Ensure `SourceBuffer` lifetime outlives tokens (parser + AST construction).

### 4) Implement CharStream abstraction
- `src/lexing/CharStream.hpp`:
  - Virtual interface: `eof()`, `peek(n)`, `get()`, `location()`.
- `src/lexing/StringViewStream.hpp/.cpp`:
  - Holds `std::string_view` + index.
  - Tracks `SourceLocation` (offset, line, column).
  - `get()` updates line/column (on `\n` -> line++, column=1).

### 5) Implement Lexer class skeleton
- `src/lexing/Lexer.hpp/.cpp`:
  - Constructor: `(std::unique_ptr<CharStream>, DiagnosticSink*, LexerOptions)`.
  - `Token next()`, `Token peek(size_t n)`, `bool eof()`.
  - Lookahead buffer `std::vector<Token>` for `peek`.
  - `iterator` input iterator (same as in `lexer_parser_api.hpp` style).

### 6) Lexing helpers
- Helper functions:
  - `bool is_alpha(char)`, `bool is_digit(char)`, `bool is_alnum(char)`, `bool is_ident_start(char)`, `bool is_ident_continue(char)`.
  - `Token make_token(TokenKind, start_loc, end_loc, lexeme_view)`.
- Use a function to consume while predicate is true to build lexeme spans.

### 7) Tokenization rules (exact order)
Implement `lex_one_()` with strict ordering:
1. **Skip whitespace and `//` comments**
   - Whitespace: spaces, tabs, newlines.
   - Comment: starts with `//`, consume to end of line (do not emit).
2. **EOF**
   - If at end, return `TokenKind::Eof` with empty lexeme.
3. **`System.out.println`**
   - If lookahead matches exact sequence, emit `KwPrintln`.
4. **Multi-char operators**: `&&`, `||`, `==`.
5. **Single-char tokens**:
   - `+ - * / ( ) { } [ ] ; < > ! . = ,`
6. **Integer literal**: `0|[1-9][0-9]*`
7. **Identifier / keyword**:
   - `[A-Za-z][A-Za-z0-9_]*`
   - Map to keyword tokens if matches reserved words.
8. **Unknown character**:
   - Emit `Invalid` token + diagnostic matching legacy message, then consume 1 char.

### 8) Keyword map
Create a static table mapping string_view to TokenKind:
- `public`, `static`, `void`, `main`, `String`, `int`, `boolean`, `if`, `else`, `while`, `length`, `true`, `false`, `this`, `new`, `return`, `class`.
- `System.out.println` handled earlier as a special case.

### 9) SourceSpan + lexeme extraction
- For each token:
  - Capture `start_loc` before consuming.
  - Compute `end_loc` after consuming.
  - `lexeme` is a view into the original buffer: `source_view.substr(start_offset, len)`.
- Ensure `StringViewStream` or `SourceBuffer` can provide raw pointer so the lexer can create views without copying.

### 10) Diagnostics parity
- Implement `DiagnosticSink` adapter that prints the legacy message:
  - `"    Line %d: lexical ('%s' symbol is not recognized by the grammar)"`
- Ensure diagnostics match formatting exactly (leading spaces, punctuation, newline).
- Maintain `lexical_error_count` for exit code compatibility.

### 11) Lex-only CLI flag
- Add a CLI option (e.g., `--lex` or `-lex`) in `src/main.cpp`.
- When present:
  - Run lexer only.
  - Print token names exactly as Flex mode does (e.g., `PLUSOP`, `INT LITERAL (123)`, `STR LITERAL 'foo'`).
  - Exit `SUCCESS` unless lexical errors occurred.

### 12) Unit and integration checks
- Use existing `unittests.py -lexical` to compare outputs.
- Compare token stream vs current Flex output on representative files in `test_files/lexical`.
- Add a small direct lexer test file (optional) that checks:
  - `System.out.println` tokenization.
  - Multi-char operators.
  - Error formatting on invalid symbol.

### 13) Removal of Flex wiring (later)
- Once lexer is stable:
  - Remove `lexer.flex` references from `Makefile`.
  - Delete `yyin`, `yylex` usage in main.
  - Ensure build still works without Flex.

## Deliverables
- `src/lexing/Token.hpp`
- `src/lexing/SourceBuffer.hpp` (or equivalent)
- `src/lexing/CharStream.hpp/.cpp`
- `src/lexing/StringViewStream.hpp/.cpp`
- `src/lexing/Lexer.hpp/.cpp`
- `main.cpp` CLI changes for lex-only mode
- Updated `Makefile` (later, once parser is done too)

## Acceptance Criteria
- Lexical output identical to existing Flex in lex-only mode.
- Lexer can stream tokens and support `peek` lookahead.
- Diagnostics match the exact legacy message for invalid symbols.
- No exceptions used; all errors handled via diagnostics and return codes.
