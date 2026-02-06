#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "ast/Node.h"
#include "lexing/Lexer.hpp"
#include "lexing/StringViewStream.hpp"
#include "lexing/Token.hpp"
#include "parsing/Parser.hpp"
#include "semantic/Class.hpp"
#include "semantic/Method.hpp"
#include "semantic/Scope.hpp"
#include "semantic/SymbolTable.hpp"

namespace {

class CollectingDiagnosticSink final : public lexing::DiagnosticSink {
  public:
    void emit(lexing::Diagnostic d) override { diagnostics.push_back(d); }

    std::vector<lexing::Diagnostic> diagnostics;
};

void assert_no_errors(const std::vector<lexing::Diagnostic> &diagnostics) {
    int error_count = 0;
    for (const auto &d : diagnostics) {
        if (d.severity == lexing::Severity::Error) {
            error_count += 1;
        }
    }
    ASSERT_EQ(error_count, 0)
        << "Unexpected diagnostic errors: " << error_count;
}

std::unique_ptr<Node> parse_program(std::string_view source) {
    CollectingDiagnosticSink diag;
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &diag);
    parsing::Parser parser(std::move(lexer), &diag);

    auto parse_result = parser.parse_goal();
    assert_no_errors(diag.diagnostics);
    EXPECT_TRUE(parse_result.has_value());
    if (!parse_result.has_value()) {
        return nullptr;
    }
    return std::move(parse_result.value());
}

const Scope *find_child_scope(const Scope *parent, std::string_view name) {
    for (const auto *child : parent->getChildren()) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

} // namespace

TEST(SymbolTable, GoldenProgram) {
    constexpr std::string_view source = R"(public class Main {
  public static void main(String[] args) {
    System.out.println(0);
  }
}

class Foo {
  int x;
  boolean flag;
  int[] arr;

  public int bar(int a, boolean b) {
    int local;
    local = a;
    return a;
  }

  public boolean baz() {
    return flag;
  }
}
)";

    auto root = parse_program(source);
    ASSERT_NE(root, nullptr);

    SymbolTable st;
    ASSERT_TRUE(root->buildTable(st));

    const Scope *program = st.getCurrentScope();
    ASSERT_NE(program, nullptr);
    EXPECT_EQ(program->getName(), "Program");

    EXPECT_EQ(program->getVariableNames(), std::set<std::string>{});
    EXPECT_EQ(program->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(program->getClassNames(),
              (std::set<std::string>{"Main", "Foo"}));

    const Scope *main_scope = find_child_scope(program, "Class: Main");
    ASSERT_NE(main_scope, nullptr);
    EXPECT_EQ(main_scope->getVariableNames(),
              (std::set<std::string>{"this"}));
    EXPECT_EQ(main_scope->getMethodNames(),
              (std::set<std::string>{"main"}));
    EXPECT_EQ(main_scope->getClassNames(), std::set<std::string>{});

    const Scope *main_method_scope =
        find_child_scope(main_scope, "Method: main");
    ASSERT_NE(main_method_scope, nullptr);
    EXPECT_EQ(main_method_scope->getVariableNames(),
              (std::set<std::string>{"args"}));
    EXPECT_EQ(main_method_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(main_method_scope->getClassNames(), std::set<std::string>{});

    const auto *main_method_record =
        dynamic_cast<Method *>(main_method_scope->getRecord());
    ASSERT_NE(main_method_record, nullptr);
    EXPECT_EQ(main_method_record->getType(), "void");

    const Scope *foo_scope = find_child_scope(program, "Class: Foo");
    ASSERT_NE(foo_scope, nullptr);
    EXPECT_EQ(foo_scope->getVariableNames(),
              (std::set<std::string>{"this", "x", "flag", "arr"}));
    EXPECT_EQ(foo_scope->getMethodNames(),
              (std::set<std::string>{"bar", "baz"}));
    EXPECT_EQ(foo_scope->getClassNames(), std::set<std::string>{});

    auto *foo_record = dynamic_cast<Class *>(foo_scope->getRecord());
    ASSERT_NE(foo_record, nullptr);
    auto *field_x = foo_record->lookupVariable("x");
    auto *field_flag = foo_record->lookupVariable("flag");
    auto *field_arr = foo_record->lookupVariable("arr");
    ASSERT_NE(field_x, nullptr);
    ASSERT_NE(field_flag, nullptr);
    ASSERT_NE(field_arr, nullptr);
    EXPECT_EQ(field_x->getType(), "int");
    EXPECT_EQ(field_flag->getType(), "boolean");
    EXPECT_EQ(field_arr->getType(), "int[]");

    const Scope *bar_scope = find_child_scope(foo_scope, "Method: bar");
    ASSERT_NE(bar_scope, nullptr);
    EXPECT_EQ(bar_scope->getVariableNames(),
              (std::set<std::string>{"a", "b", "local"}));
    EXPECT_EQ(bar_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(bar_scope->getClassNames(), std::set<std::string>{});

    const auto *bar_record =
        dynamic_cast<Method *>(bar_scope->getRecord());
    ASSERT_NE(bar_record, nullptr);
    EXPECT_EQ(bar_record->getType(), "int");
    const auto &bar_params = bar_record->getParameters();
    ASSERT_EQ(bar_params.size(), 2u);
    EXPECT_EQ(bar_params[0]->getID(), "a");
    EXPECT_EQ(bar_params[0]->getType(), "int");
    EXPECT_EQ(bar_params[1]->getID(), "b");
    EXPECT_EQ(bar_params[1]->getType(), "boolean");

    const Scope *baz_scope = find_child_scope(foo_scope, "Method: baz");
    ASSERT_NE(baz_scope, nullptr);
    EXPECT_EQ(baz_scope->getVariableNames(), std::set<std::string>{});
    EXPECT_EQ(baz_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(baz_scope->getClassNames(), std::set<std::string>{});

    const auto *baz_record =
        dynamic_cast<Method *>(baz_scope->getRecord());
    ASSERT_NE(baz_record, nullptr);
    EXPECT_EQ(baz_record->getType(), "boolean");
    EXPECT_EQ(baz_record->getParameterCount(), 0u);
}
