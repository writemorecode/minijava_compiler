# Symbol Table Visitor Refactor Spec

## 1. Purpose
This spec defines a refactor from AST-owned symbol table construction (`Node::buildTable`) to a dedicated semantic pass using the Visitor pattern. The primary goals are:

1. Centralize symbol table generation logic.
2. Make symbol table errors structured and testable.
3. Remove direct `stderr`/`stdout` side effects from symbol table construction.
4. Preserve current symbol table shape and downstream behavior.

## 2. Current Problems
The current implementation has these issues:

1. Symbol table behavior is spread across AST node classes (`ClassNode`, `MainClassNode`, `MethodNode`, `MethodWithoutParametersNode`, `MethodParameterNode`, `VariableNode`, plus recursive default in `Node`).
2. Error reporting is side-effect-only (`std::cerr` and one `std::cout`) and not inspectable as data.
3. `bool` return from `buildTable` loses error details (kind, location, count).
4. Compiler exit handling for semantic failure is inconsistent in `main.cpp`.

## 3. Scope
In scope:

1. New AST visitor infrastructure for semantic passes.
2. New `SymbolTableVisitor` pass.
3. Structured diagnostics for symbol-table errors.
4. Integration into compiler pipeline and tests.

Out of scope for this change:

1. Full type-checker migration to visitor.
2. IR generation migration to visitor.
3. Semantic language rules beyond existing symbol-table behavior.

## 4. High-Level Architecture
### 4.1 New modules
Add these files:

1. `src/ast/AstVisitor.hpp`
2. `src/semantic/SymbolTableVisitor.hpp`
3. `src/semantic/SymbolTableVisitor.cpp`

### 4.2 Updated modules
Modify these files:

1. `src/ast/Node.h`
2. `src/ast/Node.cpp`
3. `src/ast/ClassNode.hpp`
4. `src/ast/MainClassNode.hpp`
5. `src/ast/MethodNode.hpp`
6. `src/ast/MethodWithoutParametersNode.hpp`
7. `src/ast/MethodParameterNode.hpp`
8. `src/ast/VariableNode.hpp`
9. `src/main.cpp`
10. `tests/symbol_table_test.cpp`
11. `CMakeLists.txt` (if new compilation unit must be listed explicitly)

## 5. Visitor Contract
### 5.1 `AstVisitor` interface
`AstVisitor` defines overloads for relevant node types and a fallback:

```cpp
class AstVisitor {
  public:
    virtual ~AstVisitor() = default;

    virtual void visit(const Node &node);
    virtual void visit(const ClassNode &node);
    virtual void visit(const MainClassNode &node);
    virtual void visit(const MethodNode &node);
    virtual void visit(const MethodWithoutParametersNode &node);
    virtual void visit(const MethodParameterNode &node);
    virtual void visit(const VariableNode &node);
};
```

Design intent:

1. `visit(const Node&)` is default recursive traversal across `children`.
2. Specialized overloads handle symbol-table declarations with scope-aware logic.

### 5.2 `Node::accept`
Add to `Node`:

```cpp
virtual void accept(AstVisitor &visitor) const;
```

Implementation:

1. Base `Node::accept` calls `visitor.visit(*this)`.
2. Relevant subclasses override `accept` and call their typed overload.
3. Non-specialized nodes inherit base behavior.

## 6. Node Data Access Requirements
`SymbolTableVisitor` must not parse semantics from string labels like `node.type == "Method"`. It must use typed nodes.

Add const getters for symbol-table-relevant nodes:

1. `ClassNode`: class name, body node.
2. `MainClassNode`: class name, main argument name, body node.
3. `MethodNode`: method name, method return type string, params node, body node.
4. `MethodWithoutParametersNode`: method name, return type string, body node.
5. `MethodParameterNode`: parameter type string, parameter name.
6. `VariableNode`: variable type string, variable name.

Getters should return `const std::string&` and/or `const Node&` where applicable.

## 7. Diagnostics Model
### 7.1 Reuse existing diagnostics infrastructure
Reuse `lexing::Diagnostic` and `lexing::DiagnosticSink` from `src/lexing/Diagnostics.hpp`.

For semantic diagnostics:

1. `severity` is always `lexing::Severity::Error` for this pass.
2. `span.begin.line` and `span.end.line` are set from node line number.
3. `message` matches current user-facing style where practical.

### 7.2 Pass result type
Define in `SymbolTableVisitor.hpp`:

```cpp
struct SemanticPassResult {
    int error_count = 0;
    [[nodiscard]] bool ok() const { return error_count == 0; }
};
```

Public entry point:

```cpp
SemanticPassResult build_symbol_table(const Node &root, SymbolTable &table,
                                      lexing::DiagnosticSink *sink = nullptr);
```

Behavior:

1. Pass emits diagnostics to `sink` if provided.
2. Pass tracks its own `error_count` regardless of sink presence.
3. No direct printing in pass code.

## 8. Symbol Table Construction Rules
These rules preserve current semantics unless explicitly stated.

### 8.1 Program traversal
1. Start from root AST node.
2. Traverse in AST child order.
3. Build scopes and records deterministically.

### 8.2 Main class declaration
1. On duplicate class name in program scope, emit error and skip main-class subtree.
2. Otherwise:
3. Add class record.
4. Enter class scope.
5. Add `this` variable with class type and attach to class record.
6. Add `main` method with return type `void` and attach to class record.
7. Enter method scope.
8. Add main argument variable of type `String[]`.
9. Exit method scope.
10. Visit main body statements.
11. Exit class scope.

### 8.3 Regular class declaration
1. On duplicate class name in program scope, emit error and skip class body traversal.
2. Otherwise:
3. Add class record.
4. Enter class scope.
5. Add `this` variable and attach to class record.
6. Visit class body.
7. Exit class scope.

### 8.4 Method declaration with parameters
1. Duplicate check is class-scope-local.
2. On duplicate method name in current class, emit error and skip method subtree.
3. Otherwise add method record and attach to class record.
4. Enter method scope.
5. Visit parameter list and body.
6. Exit method scope.

### 8.5 Method declaration without parameters
1. Same duplicate and scope behavior as 8.4.
2. Visit body only.

### 8.6 Parameter declaration
1. Duplicate check is current method scope only.
2. On duplicate parameter name, emit error and skip insertion.
3. Otherwise add variable, then attach as method parameter.

### 8.7 Variable declaration
1. Duplicate check is current scope only.
2. On duplicate variable name, emit error and skip insertion.
3. Otherwise add variable, then attach to current record:
4. If current record is class, add as class field.
5. If current record is method, add as method local.

## 9. Error Recovery Policy
The pass must continue after recoverable errors to maximize diagnostics per run.

Rules:

1. Duplicate class: skip that class subtree, continue siblings.
2. Duplicate method: skip that method subtree, continue siblings.
3. Duplicate variable/parameter: skip insertion only, continue same subtree.
4. Scope push/pop must remain balanced even when errors occur.

Implementation requirement:

1. Use a local RAII scope guard for `enter*Scope`/`exitScope` pairs.

## 10. Backward Compatibility and Migration
### 10.1 Phase 1 migration target
1. Introduce visitor pass and switch all callers from `root->buildTable(st)` to `build_symbol_table(*root, st, sink)`.
2. Keep `Node::buildTable` temporarily as compatibility wrapper if needed.
3. Mark `Node::buildTable` and overrides deprecated in comments.

### 10.2 Phase 2 cleanup
1. Remove `buildTable` virtual function from AST base and subclasses.
2. Remove old implementations in node `.cpp` files.
3. Keep behavioral parity validated by tests.

## 11. Compiler Pipeline Integration
`main.cpp` semantic stage must change to:

1. Run symbol table pass with sink that prints diagnostics.
2. Run type checker pass only if symbol table pass is OK.
3. Return `errCodes::SEMANTIC_ERROR` on symbol-table or type-check errors.

This corrects current semantic failure exit behavior.

## 12. Test Plan
### 12.1 Existing tests to keep
Keep current golden symbol table tests and adapt them to the new API.

### 12.2 New tests to add
Add tests in `tests/symbol_table_test.cpp` for diagnostics:

1. Duplicate class emits one error with expected line.
2. Duplicate method in same class emits one error with expected line.
3. Duplicate parameter emits one error with expected line.
4. Duplicate local variable emits one error with expected line.
5. Duplicate field emits one error with expected line.

### 12.3 Test style
Use collecting sink pattern already present in tests:

1. Parse source.
2. Run `build_symbol_table` with collecting sink.
3. Assert `SemanticPassResult`.
4. Assert diagnostic count, line(s), and key message fragments.

## 13. Acceptance Criteria
The refactor is complete when all are true:

1. No symbol-table code writes directly to `std::cerr` or `std::cout`.
2. Symbol table generation is executed via `SymbolTableVisitor` pass API.
3. Existing symbol table golden tests pass with equivalent scope/record layout.
4. New negative tests validate diagnostics and recovery.
5. Compiler returns `SEMANTIC_ERROR` on symbol-table failure.

## 14. Non-Goals and Follow-Up
Non-goal now:

1. Complete migration of `checkTypes` to visitor.

Recommended follow-up:

1. Introduce `TypeCheckVisitor` with same diagnostics contract.
2. Unify semantic pass runner API for all semantic phases.
3. Replace `std::string` type-check result with structured result + diagnostics.

