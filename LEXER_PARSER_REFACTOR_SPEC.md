# Lexer/Parser Refactor Spec (Flex/Bison -> C++20)

## Context
The project currently uses Flex (`src/lexing/lexer.flex`) and Bison (`src/parsing/parser.yy`) to produce a MiniJava lexer and parser. The goal is to replace both with pure C++ (no generated code), while keeping language behavior, AST construction, and tests stable.

This spec is an initial blueprint and is informed by:
- `lexer_parser_api.hpp` (API inspiration)
- existing lexer rules (`src/lexing/lexer.flex`)
- existing grammar and AST actions (`src/parsing/parser.yy`)

## Goals
- Pure C++20 lexer and parser, no Flex/Bison dependency.
- Streaming lexer with token-iterator style API (peek/next + iterator).
- Recursive descent parser for statements, declarations, and module structure.
- Pratt parser for expressions (same operator precedence/associativity as current grammar).
- No exceptions anywhere; use explicit error return types.
- “Rust-like” style: explicit ownership, small value types, no globals, clear Result/Expected-based error paths.
- Keep AST output compatible with existing `Node` subclasses used throughout the compiler.
- Add a CLI flag to run the lexer only (replacement for `USE_LEX_ONLY`).

## Non-Goals (for initial refactor)
- Changing language features or grammar structure.
- Rewriting AST classes or type checker/IR/bytecode components.
- Improving grammar ambiguity beyond what is necessary to match current behavior.

## Summary of Current Language Surface (from lexer.flex / parser.yy)
### Keywords / Reserved identifiers
`public`, `static`, `void`, `main`, `String`, `int`, `boolean`, `if`, `else`, `while`, `System.out.println`, `length`, `true`, `false`, `this`, `new`, `return`, `class`

### Operators / punctuation
`+ - * / ( ) { } [ ] ; < > && || == ! . = ,`

### Identifiers and literals
- `ID`: `[A-Za-z][A-Za-z0-9_]*`
- `INT_LITERAL`: `0|[1-9][0-9]*`
- Comments: `//...` to end of line
- Whitespace: spaces, tabs, newlines

### Statements / declarations (RD candidates)
- Program = `MainClass` + `ClassDeclaration*`
- Class declaration, var declarations, method declarations
- Statements: block, if/else, while, println, assignment, array assignment

### Expressions (Pratt candidates)
- Binary: `+ - * / || && < > ==`
- Postfix: `expr[expr]`, `expr.length`, `expr.id()`, `expr.id(exprlist)`
- Primary: integer, true/false, identifier, `this`, `new int[expr]`, `new Identifier()`, `(expr)`
- Prefix: `!` (legacy low precedence), unary `-` (planned behavior fix)

## Proposed Architecture

### Shared Types (new module)
Add a shared header in `src/lexing/` (or `src/util/`) for token and diagnostic types. This mirrors `lexer_parser_api.hpp` but is tailored for MiniJava.

#### Source location
- Track `line`, `column`, and byte `offset` while lexing.
- For AST nodes, continue to store only `lineno` (int) for compatibility; parser should pass `token.span.begin.line` where a line number is required.

```cpp
struct SourceLocation { size_t offset, line, column; };
struct SourceSpan { SourceLocation begin, end; };
```

#### Diagnostics (no exceptions)
```cpp
enum class Severity : uint8_t { Note, Warning, Error };
struct Diagnostic { Severity severity; std::string message; SourceSpan span; };
class DiagnosticSink { public: virtual ~DiagnosticSink() = default; virtual void emit(Diagnostic d)=0; };
```

#### Token kinds (MiniJava-specific)
```cpp
enum class TokenKind : uint16_t {
  Eof, Invalid,

  // Identifiers & literals
  Identifier, IntLiteral,

  // Keywords
  KwPublic, KwStatic, KwVoid, KwMain, KwString, KwInt, KwBoolean,
  KwIf, KwElse, KwWhile, KwPrintln, KwLength, KwTrue, KwFalse,
  KwThis, KwNew, KwReturn, KwClass,

  // Punctuation / operators
  LParen, RParen, LCurly, RCurly, LSquare, RSquare,
  Semi, Comma, Dot,
  Plus, Minus, Star, Slash,
  Assign, EqEq, Lt, Gt,
  AndAnd, OrOr, Bang,
};
```

#### Token payload
```cpp
using TokenValue = std::variant<std::monostate, int64_t, std::string_view>;
struct Token { TokenKind kind; std::string_view lexeme; SourceSpan span; TokenValue value; };
```

Notes:
- `lexeme` should remain valid at least until the token has been consumed by the parser.
- Identifiers **must** be `std::string_view` pointing into the original program source buffer. This requires the lexer input buffer to outlive all tokens (e.g., the full file is read into memory and wrapped by `StringViewStream` or equivalent).

### CharStream abstraction (streaming input)
Mirror `lexer_parser_api.hpp`:
```cpp
class CharStream {
  public:
    virtual ~CharStream() = default;
    virtual bool eof() const = 0;
    virtual char peek(size_t lookahead = 0) const = 0;
    virtual char get() = 0;
    virtual SourceLocation location() const = 0;
};
```

Concrete implementations:
- `StringViewStream` for tests and production use (backed by a full, owned source buffer).
- `FileStream` may still be useful for truly streaming input, but if used, it must keep a stable backing buffer to satisfy the `string_view` lifetime requirement.

### Lexer
Core API (inspired by `lexer_parser_api.hpp`):
- `Token next()`
- `Token peek(size_t n=0)`
- `bool eof()`
- `iterator begin()/end()` (input iterator)

Lexing rules (longest match, then keyword/identifier resolution):
1. Skip whitespace and `//` comments (no trivia tokens in initial refactor).
2. Recognize `System.out.println` as a **single** `KwPrintln` token before `Identifier`/`Dot` rules.
3. Recognize multi-char operators `&&`, `||`, `==` before single-char operators.
4. `INT_LITERAL` and `ID` as defined above.
5. Unknown characters -> `Invalid` token + diagnostic (and continue to next char).

Token values:
- `IntLiteral` uses `int64_t` value (range checks optional for initial pass).
- `Identifier` stores `std::string_view` into the original program source buffer.

Error handling:
- Emit diagnostics via `DiagnosticSink` with the **same message text** currently produced by Flex/Bison (for test compatibility).
- Maintain `lexical_error_count` in the lexer or an external error collector. Main should check and return `LEXICAL_ERROR` similar to current behavior.

### Parser
Parser owns a `Lexer` instance (by value, as in `lexer_parser_api.hpp`) and provides explicit `Result<T>` return types (no exceptions).

#### Result type
Since the project is C++20 (no `std::expected`), introduce a small `Expected<T, E>` in `src/util/Expected.hpp` or equivalent.

```cpp
struct ParseError { std::string message; SourceSpan span; };
template <class T> using Result = Expected<T, ParseError>;
```

#### Entry points
- `Result<Node*> parse_goal()`
- `Result<Node*> parse_main_class()`
- `Result<Node*> parse_class_decl()`
- `Result<Node*> parse_stmt()`
- `Result<Node*> parse_expr()`

Parser should expose a `done()` / `has_errors()` method to match existing main flow.

#### Recursive descent structure
Map each nonterminal in `parser.yy` to a function, keeping AST construction equivalent:
- `Goal`, `MainClass`, `ClassDeclaration`, `ClassDeclarationList`
- `ClassBody`, `ClassVarDeclarationList`, `MethodDeclarationList`
- `VarDeclaration`, `MethodDeclaration`, `MethodBody`, `MethodBodyItemList`
- `StatementList`, `Statement`, `ExpressionList`, `Type`, `Identifier`, `Integer`

AST wrapper nodes created by the Bison grammar (must be preserved):
- `Node("Class declaration list", ...)` when multiple class declarations exist.
- `Node("Variable declaration list", ...)`, `Node("Method declaration list", ...)` for class members.
- `Node("Statement list", ...)` for statement lists; block statements return the list node directly.
- `Node("Expression list", ...)` for method call arguments.
- `Node("Method parameter list", ...)` for method parameters.
- `Node("Method body item list", ...)`, `Node("Variable declaration", ...)`, `Node("Statement", ...)` inside method bodies.
- `Node("Empty class body", ...)` and `Node("Class body", ...)` for specific class body forms.
- `Node("Empty statement", ...)` for `{}` as a statement.

Root behavior:
- If the program has only a `MainClass`, `root` is the `MainClassNode`.
- If additional classes exist, `root` is the `Class declaration list` node with `MainClassNode` inserted at the front.
- `if`/`else` should bind the `else` to the nearest unmatched `if` (same effect as Bison’s `%right "then" ELSE`).

#### Pratt expression parsing
Maintain precedence/associativity from the Bison grammar, **including its unusual NOT precedence**:

| Level (low -> high) | Operators / forms | Assoc |
| --- | --- | --- |
| 1 | `||` | right (Bison uses `%right OR`) |
| 2 | `&&` | right |
| 3 | `<` `>` `==` `!` | left (NOTE: `NOT` is declared with comparisons in `parser.yy`) |
| 4 | `+` `-` | left |
| 5 | `*` `/` | left |
| 6 | postfix: `expr[expr]`, `.length`, `.id(...)` | left |
| 7 | prefix: unary `-` | right (explicit **behavior fix**) |
| 8 | primary: literals, identifiers, `this`, `new ...`, `(expr)` | n/a |

Notes:
- This implies `!a + b` parses as `!(a + b)` with the legacy grammar; keep this for compatibility.
- Unary `-` is the only intentional behavior change: it becomes a real prefix operator (instead of `- INT_LITERAL` creating an `IdentifierNode`).
- Bison declares `DOT` higher than `L_PAREN`/`L_SQUARE`; in Pratt this can be represented with a slightly higher binding power for dot-postfix, or by treating all postfix operators as equally highest if no ambiguity is observed in tests.

Implementation detail:
- Use `nud` for prefix/primary forms.
- Use `led` for binary and postfix forms.
- Postfix forms should bind tighter than binary ops.

#### AST construction (match current nodes)
Use the same node types created in `parser.yy` actions:
- `MainClassNode`, `ClassNode`, `MethodNode`, `MethodWithoutParametersNode`, etc.
- Expression nodes: `PlusNode`, `MinusNode`, `MultiplicationNode`, `DivisionNode`, `OrNode`, `AndNode`, `LessThanNode`, `GreaterThanNode`, `EqualToNode`
- Postfix nodes: `ArrayAccessNode`, `ArrayLengthNode`, `MethodCallNode`, `MethodCallWithoutArgumentsNode`
- Literals and identifiers: `IntegerNode`, `IdentifierNode`, `TrueNode`, `FalseNode`, `ThisNode`
- Allocation: `IntegerArrayAllocationNode`, `ClassAllocationNode`
- Statements: `IfNode`, `IfElseNode`, `WhileNode`, `PrintNode`, `AssignNode`, `ArrayAssignNode`, `ReturnOnlyMethodBodyNode`, `MethodBodyNode`

Line numbers should be drawn from the first relevant token in each construct (matching how Bison passed `yylineno`).

### Error handling & recovery
- The current Bison parser effectively reports **one** syntax error (no error-recovery rules).
- To keep outputs identical, the new parser should **stop after the first syntax error** and emit the same text.
- Optional recovery (sync at `;` / `}` / `class`) can be added later behind a flag, but is out of scope for parity.

### Integration points
- `src/main.cpp` currently uses `yy::parser` and `yylex()`. It will need to be updated to use the new `Parser` class and to report errors similarly.
- Replace or remove `lexer.flex` / `parser.yy` and the generated `parser.tab.hh` usage.
- Update `CMakeLists.txt` to drop Flex/Bison steps and include new C++ compilation units.

## Proposed File Layout (initial pass)
- `src/lexing/Token.hpp` (TokenKind, Token, SourceSpan, diagnostics)
- `src/lexing/CharStream.hpp` / `CharStream.cpp`
- `src/lexing/Lexer.hpp` / `Lexer.cpp`
- `src/parsing/Parser.hpp` / `Parser.cpp`
- `src/util/Expected.hpp` (if needed for C++20)

## Decisions Locked In
1. **Unary minus behavior**: fix the current incorrect behavior and implement proper unary `-` in the Pratt parser (no more `IdentifierNode("-$2", ...)`).
2. **Identifier storage**: tokens use `std::string_view` into the original program source buffer; the buffer must outlive all tokens.
3. **Diagnostic formatting**: keep error messages unchanged for now (match current output).
4. **Lex-only mode**: add a CLI flag that runs only the lexer and prints token names (replacement for `USE_LEX_ONLY`).

## Compatibility Checklist
- Same token set and keyword recognition (notably `System.out.println`).
- Same operator precedence and associativity for expressions.
- Same AST node types and construction patterns.
- Same wrapper-node structure and root selection (`MainClass` vs `Class declaration list`).
- Same error code behavior in `main.cpp` (`LEXICAL_ERROR` vs `SYNTAX_ERROR`).
