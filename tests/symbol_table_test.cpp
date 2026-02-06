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

[[maybe_unused]] constexpr std::string_view kGoldenProgram2Source = R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Point().sum(1, 2));
    System.out.println(new Point().isOrigin());
    System.out.println(new Point().self().sum(3, 4));
    if (new Util().greater(1, 2)) {
      System.out.println(1);
    } else {
      System.out.println(0);
    }
  }
}

class Point {
  int x;
  int y;
  int[] history;
  Util util;

  public int sum(int a, int b) {
    int tmp;
    tmp = a + b;
    return tmp;
  }

  public int move(int dx, int dy) {
    x = x + dx;
    y = y + dy;
    return x + y;
  }

  public boolean isOrigin() {
    return x == 0 && y == 0;
  }

  public int[] setHistory(int n) {
    int i;
    history = new int[n];
    i = 0;
    while (i < n) {
      history[i] = i;
      i = i + 1;
    }
    return history;
  }

  public int historySize() {
    return history.length;
  }

  public Point self() {
    return this;
  }
}

class Util {
  public boolean greater(int a, int b) {
    return a < b;
  }

  public int max(int a, int b) {
    int m;
    if (a < b) {
      m = b;
    } else {
      m = a;
    }
    return m;
  }
}
)";

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

TEST(SymbolTable, GoldenProgram2) {
    auto root = parse_program(kGoldenProgram2Source);
    ASSERT_NE(root, nullptr);

    SymbolTable st;
    ASSERT_TRUE(root->buildTable(st));

    const Scope *program = st.getCurrentScope();
    ASSERT_NE(program, nullptr);
    EXPECT_EQ(program->getName(), "Program");
    EXPECT_EQ(program->getClassNames(),
              (std::set<std::string>{"Main", "Point", "Util"}));
    EXPECT_EQ(program->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(program->getVariableNames(), std::set<std::string>{});
    EXPECT_EQ(program->getChildren().size(), 3u);

    const Scope *main_scope = find_child_scope(program, "Class: Main");
    ASSERT_NE(main_scope, nullptr);
    EXPECT_EQ(main_scope->getVariableNames(),
              (std::set<std::string>{"this"}));
    EXPECT_EQ(main_scope->getMethodNames(),
              (std::set<std::string>{"main"}));
    EXPECT_EQ(main_scope->getClassNames(), std::set<std::string>{});
    EXPECT_EQ(main_scope->getChildren().size(), 1u);

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
    EXPECT_EQ(main_method_record->getParameterCount(), 0u);

    const Scope *point_scope = find_child_scope(program, "Class: Point");
    ASSERT_NE(point_scope, nullptr);
    EXPECT_EQ(point_scope->getVariableNames(),
              (std::set<std::string>{"this", "x", "y", "history", "util"}));
    EXPECT_EQ(point_scope->getMethodNames(),
              (std::set<std::string>{"sum",
                                     "move",
                                     "isOrigin",
                                     "setHistory",
                                     "historySize",
                                     "self"}));
    EXPECT_EQ(point_scope->getClassNames(), std::set<std::string>{});
    EXPECT_EQ(point_scope->getChildren().size(), 6u);

    auto *point_record = dynamic_cast<Class *>(point_scope->getRecord());
    ASSERT_NE(point_record, nullptr);
    auto *field_x = point_record->lookupVariable("x");
    auto *field_y = point_record->lookupVariable("y");
    auto *field_history = point_record->lookupVariable("history");
    auto *field_util = point_record->lookupVariable("util");
    ASSERT_NE(field_x, nullptr);
    ASSERT_NE(field_y, nullptr);
    ASSERT_NE(field_history, nullptr);
    ASSERT_NE(field_util, nullptr);
    EXPECT_EQ(field_x->getType(), "int");
    EXPECT_EQ(field_y->getType(), "int");
    EXPECT_EQ(field_history->getType(), "int[]");
    EXPECT_EQ(field_util->getType(), "Util");

    const Scope *sum_scope = find_child_scope(point_scope, "Method: sum");
    ASSERT_NE(sum_scope, nullptr);
    EXPECT_EQ(sum_scope->getVariableNames(),
              (std::set<std::string>{"a", "b", "tmp"}));
    EXPECT_EQ(sum_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(sum_scope->getClassNames(), std::set<std::string>{});

    const auto *sum_record =
        dynamic_cast<Method *>(sum_scope->getRecord());
    ASSERT_NE(sum_record, nullptr);
    EXPECT_EQ(sum_record->getType(), "int");
    const auto &sum_params = sum_record->getParameters();
    ASSERT_EQ(sum_params.size(), 2u);
    EXPECT_EQ(sum_params[0]->getID(), "a");
    EXPECT_EQ(sum_params[0]->getType(), "int");
    EXPECT_EQ(sum_params[1]->getID(), "b");
    EXPECT_EQ(sum_params[1]->getType(), "int");

    const Scope *move_scope = find_child_scope(point_scope, "Method: move");
    ASSERT_NE(move_scope, nullptr);
    EXPECT_EQ(move_scope->getVariableNames(),
              (std::set<std::string>{"dx", "dy"}));
    EXPECT_EQ(move_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(move_scope->getClassNames(), std::set<std::string>{});

    const auto *move_record =
        dynamic_cast<Method *>(move_scope->getRecord());
    ASSERT_NE(move_record, nullptr);
    EXPECT_EQ(move_record->getType(), "int");
    const auto &move_params = move_record->getParameters();
    ASSERT_EQ(move_params.size(), 2u);
    EXPECT_EQ(move_params[0]->getID(), "dx");
    EXPECT_EQ(move_params[0]->getType(), "int");
    EXPECT_EQ(move_params[1]->getID(), "dy");
    EXPECT_EQ(move_params[1]->getType(), "int");

    const Scope *is_origin_scope =
        find_child_scope(point_scope, "Method: isOrigin");
    ASSERT_NE(is_origin_scope, nullptr);
    EXPECT_EQ(is_origin_scope->getVariableNames(), std::set<std::string>{});
    EXPECT_EQ(is_origin_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(is_origin_scope->getClassNames(), std::set<std::string>{});

    const auto *is_origin_record =
        dynamic_cast<Method *>(is_origin_scope->getRecord());
    ASSERT_NE(is_origin_record, nullptr);
    EXPECT_EQ(is_origin_record->getType(), "boolean");
    EXPECT_EQ(is_origin_record->getParameterCount(), 0u);

    const Scope *set_history_scope =
        find_child_scope(point_scope, "Method: setHistory");
    ASSERT_NE(set_history_scope, nullptr);
    EXPECT_EQ(set_history_scope->getVariableNames(),
              (std::set<std::string>{"n", "i"}));
    EXPECT_EQ(set_history_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(set_history_scope->getClassNames(), std::set<std::string>{});

    const auto *set_history_record =
        dynamic_cast<Method *>(set_history_scope->getRecord());
    ASSERT_NE(set_history_record, nullptr);
    EXPECT_EQ(set_history_record->getType(), "int[]");
    const auto &set_history_params = set_history_record->getParameters();
    ASSERT_EQ(set_history_params.size(), 1u);
    EXPECT_EQ(set_history_params[0]->getID(), "n");
    EXPECT_EQ(set_history_params[0]->getType(), "int");

    const Scope *history_size_scope =
        find_child_scope(point_scope, "Method: historySize");
    ASSERT_NE(history_size_scope, nullptr);
    EXPECT_EQ(history_size_scope->getVariableNames(),
              std::set<std::string>{});
    EXPECT_EQ(history_size_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(history_size_scope->getClassNames(), std::set<std::string>{});

    const auto *history_size_record =
        dynamic_cast<Method *>(history_size_scope->getRecord());
    ASSERT_NE(history_size_record, nullptr);
    EXPECT_EQ(history_size_record->getType(), "int");
    EXPECT_EQ(history_size_record->getParameterCount(), 0u);

    const Scope *self_scope = find_child_scope(point_scope, "Method: self");
    ASSERT_NE(self_scope, nullptr);
    EXPECT_EQ(self_scope->getVariableNames(), std::set<std::string>{});
    EXPECT_EQ(self_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(self_scope->getClassNames(), std::set<std::string>{});

    const auto *self_record =
        dynamic_cast<Method *>(self_scope->getRecord());
    ASSERT_NE(self_record, nullptr);
    EXPECT_EQ(self_record->getType(), "Point");
    EXPECT_EQ(self_record->getParameterCount(), 0u);

    const Scope *util_scope = find_child_scope(program, "Class: Util");
    ASSERT_NE(util_scope, nullptr);
    EXPECT_EQ(util_scope->getVariableNames(),
              (std::set<std::string>{"this"}));
    EXPECT_EQ(util_scope->getMethodNames(),
              (std::set<std::string>{"greater", "max"}));
    EXPECT_EQ(util_scope->getClassNames(), std::set<std::string>{});
    EXPECT_EQ(util_scope->getChildren().size(), 2u);

    const Scope *greater_scope =
        find_child_scope(util_scope, "Method: greater");
    ASSERT_NE(greater_scope, nullptr);
    EXPECT_EQ(greater_scope->getVariableNames(),
              (std::set<std::string>{"a", "b"}));
    EXPECT_EQ(greater_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(greater_scope->getClassNames(), std::set<std::string>{});

    const auto *greater_record =
        dynamic_cast<Method *>(greater_scope->getRecord());
    ASSERT_NE(greater_record, nullptr);
    EXPECT_EQ(greater_record->getType(), "boolean");
    const auto &greater_params = greater_record->getParameters();
    ASSERT_EQ(greater_params.size(), 2u);
    EXPECT_EQ(greater_params[0]->getID(), "a");
    EXPECT_EQ(greater_params[0]->getType(), "int");
    EXPECT_EQ(greater_params[1]->getID(), "b");
    EXPECT_EQ(greater_params[1]->getType(), "int");

    const Scope *max_scope = find_child_scope(util_scope, "Method: max");
    ASSERT_NE(max_scope, nullptr);
    EXPECT_EQ(max_scope->getVariableNames(),
              (std::set<std::string>{"a", "b", "m"}));
    EXPECT_EQ(max_scope->getMethodNames(), std::set<std::string>{});
    EXPECT_EQ(max_scope->getClassNames(), std::set<std::string>{});

    const auto *max_record =
        dynamic_cast<Method *>(max_scope->getRecord());
    ASSERT_NE(max_record, nullptr);
    EXPECT_EQ(max_record->getType(), "int");
    const auto &max_params = max_record->getParameters();
    ASSERT_EQ(max_params.size(), 2u);
    EXPECT_EQ(max_params[0]->getID(), "a");
    EXPECT_EQ(max_params[0]->getType(), "int");
    EXPECT_EQ(max_params[1]->getID(), "b");
    EXPECT_EQ(max_params[1]->getType(), "int");

    const auto root_type = root->checkTypes(st);
    EXPECT_EQ(root_type, "void");
}
