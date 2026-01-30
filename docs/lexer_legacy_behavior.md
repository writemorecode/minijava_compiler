# Legacy Lexer Behavior (Flex)

This summarizes the current Flex rules in `src/lexing/lexer.flex` for parity with the new C++ lexer.

## Tokens (in rule order)
- Operators/punctuation: `+ - * / ( ) { } [ ] ; < > && || == ! . = ,`
- Keywords: `public static void main String int boolean if else while System.out.println length true false this new return class`
- Integer literal: `0|[1-9][0-9]*`
- Identifier: `[A-Za-z][A-Za-z0-9_]*`
- Comment: `//` to end of line (ignored)
- Whitespace: spaces, tabs, newlines (ignored)

## Special cases
- `System.out.println` is a single keyword token, not `Identifier . Identifier . Identifier`.
- Multi-char operators (`&&`, `||`, `==`) must be matched before single-char operators.

## Legacy error output
On unrecognized characters, Flex prints:

```
    Line <line>: lexical ('<char>' symbol is not recognized by the grammar)
```

and sets `lexical_errors = 1`.
