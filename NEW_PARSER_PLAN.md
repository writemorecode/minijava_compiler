# New Parser Implementation Plan

## Scope
Replace the current Bison parser with a pure C++20 parser that:
- Consumes the new streaming lexer (`src/lexing/Lexer.hpp`).
- Uses Pratt parsing for expressions and recursive descent for all other constructs.
- Produces the same AST node types and line numbers as the existing grammar actions.
- Preserves current diagnostic text and error behavior (per spec).
- Fixes unary minus behavior (true prefix `-` binding tighter than `* /`).

This plan assumes the lexer has already been implemented and integrated behind a `--lex` flag.

## References
- Existing grammar and AST actions: `src/parsing/parser.yy`
- Lexer tokens and diagnostics: `src/lexing/lexer.flex`, `src/lexing/Lexer.*`
- Spec: `LEXER_PARSER_REFACTOR_SPEC.md`
- Build system: `CMakeLists.txt`

## High-Level Steps

### 1) Introduce Result/Expected utility
**Goal:** A no-exceptions error carrier for parsing routines.
- Add `src/util/Expected.hpp` with a small `Expected<T, E>` or `Result<T>` helper.
- Ensure it supports: `ok(value)`, `err(error)`, `has_value()`, `value()`, `error()`.
- Keep it header-only and C++20-friendly.

Deliverables:
- `src/util/Expected.hpp`

### 2) Define Parser interface + core types
**Goal:** Provide a clean, testable API similar in style to Rust.
- Add `src/parsing/Parser.hpp` declaring:
  - `struct ParseError { std::string message; lexing::SourceSpan span; };`
  - `class Parser` with:
    - `Parser(lexing::Lexer lexer, lexing::DiagnosticSink *sink)`
    - `Result<Node *> parse_goal()` (top-level entry)
    - `bool has_errors() const`
    - `int error_count() const`
- Decide on token buffer management:
  - Use a small lookahead buffer (e.g., `std::deque<Token>`) to support `peek(n)`.
  - Maintain `current()` and `advance()` helpers.

Deliverables:
- `src/parsing/Parser.hpp`

### 3) Parser helpers and synchronization
**Goal:** Uniform token handling and error behavior parity with Bison.
- Implement helpers in `Parser.cpp`:
  - `const Token &peek(size_t n = 0)`
  - `Token consume()`
  - `bool match(TokenKind)`
  - `Result<Token> expect(TokenKind, std::string_view expected_label)`
- Implement error reporting:
  - Reuse diagnostic text from current `yy::parser::error` output (same strings).
  - Stop after the **first** syntax error (Bison has no recovery rules).
- Defer any recovery/synchronization to a later, opt-in improvement.

Deliverables:
- `src/parsing/Parser.cpp` (helper sections + error handling)

### 4) Pratt expression parser
**Goal:** Match precedence/associativity from `parser.yy` and fix unary minus.
- Implement `parse_expr(min_bp)` using Pratt (`nud/led` or binding-power table).
- Define binding powers:
  - `||` (right-assoc), `&&` (right-assoc)
  - `< > == !` (left-assoc; `!` is grouped with comparisons in `parser.yy`)
  - `+ -` (left-assoc)
  - `* /` (left-assoc)
  - postfix: `[...]`, `.length`, `.id(...)` (left-assoc, highest)
    - Note: Bison declares `DOT` higher than `L_PAREN`/`L_SQUARE`; if needed, give dot-postfix slightly higher binding power.
  - prefix: unary `-` (right-assoc, higher than binary) **(behavior fix)**
- Construct AST nodes identical to Bison actions.
- Ensure unary `-` produces `MinusNode` or a dedicated node (if present) with `0 - expr` strategy if necessary.

Deliverables:
- Pratt implementation in `src/parsing/Parser.cpp`.

### 5) Recursive descent for statements and declarations
**Goal:** Port the grammar to explicit methods.
Implement methods mapping to `parser.yy`:
- `parse_goal`, `parse_main_class`, `parse_class_decl`, `parse_class_body`
- `parse_var_decl`, `parse_method_decl`, `parse_method_body`, `parse_method_body_item`
- `parse_statement`, `parse_statement_list`
- `parse_expression_list`, `parse_type`, `parse_identifier`, `parse_integer`

Keep AST construction consistent with current node creation:
- Use the same node classes: `MainClassNode`, `ClassNode`, `MethodNode`, etc.
- Preserve line number sourcing (use first token of the construct).
- Preserve wrapper nodes exactly as in Bison:
  - `Node("Statement list", ...)` for statement lists; `{ ... }` returns the list node itself.
  - `Node("Empty statement", ...)` for `{}`.
  - `Node("Expression list", ...)`, `Node("Method parameter list", ...)`.
  - `Node("Method body item list", ...)`, `Node("Variable declaration", ...)`, `Node("Statement", ...)`.
  - `Node("Variable declaration list", ...)`, `Node("Method declaration list", ...)`, `Node("Class declaration list", ...)`.
  - `Node("Empty class body", ...)`, `Node("Class body", ...)`.
- Root behavior: `MainClass` alone -> `root = MainClassNode`; otherwise `root = Class declaration list` with main inserted at front.
- Ensure dangling-else attaches to the nearest `if` (equivalent to Bison's `%right "then" ELSE`).

Deliverables:
- RD implementation in `src/parsing/Parser.cpp`.

### 6) Integrate parser with compiler entrypoint
**Goal:** Replace Bison parser usage with new Parser class.
- Update `src/main.cpp`:
  - Remove `parser.tab.hh` include.
  - Replace `yy::parser` usage with `Parser` and `Lexer`.
  - Hook diagnostics to preserve existing error messages.
  - Preserve `lexical_errors` behavior (lex-first fail if lexer errors).
- Ensure `root` is set with the new parse result for downstream stages.

Deliverables:
- `src/main.cpp` updated for new parser.

### 7) Remove Flex/Bison build integration
**Goal:** Delete generator steps and generated code from the build.
- Update `CMakeLists.txt`:
  - Remove `find_package(BISON/FLEX)`
  - Remove `BISON_TARGET`, `FLEX_TARGET`, and dependency wiring.
  - Remove generated sources from `compiler` target.
- Remove or leave `src/parsing/parser.yy` and `src/lexing/lexer.flex` as legacy reference (decide per project preference).

Deliverables:
- `CMakeLists.txt` updated.
- Optionally move legacy grammar files to `docs/`.

### 8) Error-message parity checks
**Goal:** Ensure tests still pass with identical error text.
- Compare existing outputs for syntax errors:
  - `Syntax errors found! See the logs below:`
  - `\t@error at line X. Cannot generate a syntax for this input:...`
  - `End of syntax errors!`
- Ensure only **one** syntax error report is emitted (Bison default).
- Ensure lexer errors still block syntax parsing (same behavior as `main.cpp` today).

Deliverables:
- Parser diagnostics alignment verified.

### 9) Validation and tests
**Goal:** Match current behavior across test suites.
- Build: `make compiler` or `cmake --build ...`.
- Run `python unittests.py -lexical -syntax -semantic -valid`.
- Add or update tests if unary minus fix changes behavior:
  - Consider a targeted `test_files/` case for `-x` and `-(1+2)`.

Deliverables:
- Passing test suite (or documented deltas).

## Notes / Implementation Details
- Identifiers should stay as `std::string_view` into the `SourceBuffer`.
- Parser must not outlive the `SourceBuffer` used by lexer, or it should store a `shared_ptr` to the buffer.
- No exceptions: all failure paths return `Result<T>` and use diagnostics for user-facing output.
- Keep AST `lineno` values consistent with existing nodes (use the token line where Bison used `yylineno`).

## Checklist (definition of done)
- [ ] Parser compiles without Bison/Flex.
- [ ] All AST nodes are produced as before.
- [ ] Unary minus fixed with proper Pratt binding.
- [ ] Error messages unchanged.
- [ ] CMake builds without generator dependencies.
- [ ] Test harness passes.
