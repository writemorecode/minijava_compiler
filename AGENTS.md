# Repository Guidelines

## Project Structure & Module Organization
- `src/` holds the compiler and VM C++ sources, organized by domain (`ast/`, `lexing/`, `parsing/`, `semantic/`, `ir/`, `bytecode/`, `vm/`, `util/`).
- `build/` is generated output (objects, binaries, generated parser/lexer code).
- `output/` holds compiled bytecode artifacts (e.g., `output/prog.bc`) and Graphviz `.dot` outputs.
- `test_files/` contains MiniJava programs grouped by purpose (valid, lexical/syntax/semantic errors).
- `docs/grammar.g` is the language grammar reference.

## Build, Test, and Development Commands
- `make compiler` builds the compiler binary at `build/bin/compiler`.
- `make vm` builds the VM binary at `build/bin/vm`.
- `make` builds both targets.
- `./build/bin/compiler path/to/Program.java` compiles to `output/prog.bc`.
- `./build/bin/vm output/prog.bc` runs the compiled program.
- `python unittests.py -lexical -syntax -semantic -valid` runs the test harness over test_files.
- `make tree`, `make st`, `make cfg` render Graphviz PDFs from `output/*.dot`.
- `make fmt` runs `clang-format` over `src/`.
- `make clean` removes `build/` and generated PDFs.

## Coding Style & Naming Conventions
- C++23 with 4-space indentation and braces on the same line as control statements.
- Prefer matching file and type names (e.g., `ClassNode.hpp` / `ClassNode.cpp`).
- AST node classes use `PascalCase` with a `Node` suffix; headers typically use `.hpp`.
- Run `make fmt` before submitting changes.

## Testing Guidelines
- Test cases live in `test_files/**`. Error expectations are annotated as `// @error - message` in `.java` files.
- The Python harness compares expected error line numbers against compiler output.
- Add new tests to the correct category directory and keep filenames descriptive.

## Commit & Pull Request Guidelines
- Commit messages in history are short, imperative, and sometimes prefixed (e.g., `Fix: ...`, `Added ...`, `WIP: ...`). Follow this pattern.
- PRs should explain the change, list tests run (or note if none), and link any relevant issues.

## Tooling & Dependencies
- Requires `g++`, `bison`, `flex`, `clang-format`, and `graphviz` (`dot`).
