#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ast/Node.h"
#include "lexing/Lexer.hpp"
#include "lexing/StringViewStream.hpp"
#include "lexing/Token.hpp"
#include "parsing/Parser.hpp"

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
        << "Parser reported " << error_count << " error(s).";
}

struct ExpectedNode {
    std::string type;
    std::string value;
    std::vector<ExpectedNode> children;
};

ExpectedNode node(std::string type, std::string value = "",
                  std::vector<ExpectedNode> children = {}) {
    return ExpectedNode{.type=std::move(type), .value=std::move(value), .children=std::move(children)};
}

ExpectedNode id(std::string name) { return node("Identifier", std::move(name)); }

ExpectedNode type_node(std::string name) { return node("Type", std::move(name)); }

ExpectedNode integer(std::string value) { return node("Integer", std::move(value)); }

ExpectedNode unary(std::string type, ExpectedNode expr) {
    return node(std::move(type), "", {std::move(expr)});
}

ExpectedNode binary(std::string type, ExpectedNode left, ExpectedNode right) {
    return node(std::move(type), "", {std::move(left), std::move(right)});
}

ExpectedNode expected_tree() {
    ExpectedNode const arithmetic_expr = binary(
        "Minus", binary("Plus", integer("1"), integer("2")),
        binary("Division", binary("Multiplication", integer("3"), integer("4")),
               integer("5")));

    ExpectedNode const complex_condition =
        binary("OR",
               binary("AND", unary("Negated expression", id("flag")),
                      binary("Less-than", id("a"), id("b"))),
               binary("Greater-than", id("a"), id("b")));

    ExpectedNode const main_statements = node(
        "Statement list", "",
        {
            node("Assign", "", {id("a"), arithmetic_expr}),
            node("Assign", "",
                 {id("b"), binary("Minus", integer("0"), integer("1"))}),
            node("Assign", "", {id("flag"), node("TRUE")}),
            node("Assign", "", {id("flag"), node("FALSE")}),
            node("If", "", {complex_condition, node("Empty statement")}),
            node("If-else", "",
                 {
                     binary("EQ", id("a"), id("b")),
                     node("Statement list", "",
                          {node("Print", "", {integer("0")})}),
                     node("Empty statement"),
                 }),
            node("While", "",
                 {
                     binary("Less-than", id("b"), integer("10")),
                     node("Statement list", "",
                          {node("Assign", "",
                                {id("b"),
                                 binary("Plus", id("b"), integer("1"))})}),
                 }),
            node("Assign", "", {id("arr"), node("Integer array allocation")}),
            node("Assign", "", {id("a"), node("Array length")}),
            node(
                "Assign", "",
                {id("b"), node("Array access", "", {id("arr"), integer("1")})}),
            node("Assign", "", {id("foo"), node("Class allocation", "Foo")}),
            node("Assign", "",
                 {id("a"), node("Method call", "",
                                {id("foo"), id("bar"),
                                 node("Expression list", "",
                                      {integer("8"), integer("9")})})}),
            node("Assign", "",
                 {id("a"), node("Method call", "", {id("foo"), id("baz")})}),
            node("Print", "", {id("a")}),
            node("Array assign", "", {id("arr"), integer("2")}),
        });

    ExpectedNode const foo_fields =
        node("Variable declaration list", "",
             {
                 node("Variable", "", {type_node("int"), id("x")}),
                 node("Variable", "", {type_node("boolean"), id("flag")}),
                 node("Variable", "", {type_node("int[]"), id("arr")}),
                 node("Variable", "", {type_node("Foo"), id("other")}),
             });

    ExpectedNode const bar_method = node(
        "Method", "",
        {type_node("int"), id("bar"),
         node("Method parameter list", "",
              {
                  node("Method parameter", "", {type_node("int"), id("x")}),
                  node("Method parameter", "", {type_node("int"), id("y")}),
              }),
         node(
             "Method body", "",
             {node("Method body item list", "",
                   {node("Variable declaration", "",
                         {node("Variable", "", {type_node("int"), id("z")})}),
                    node("Statement", "",
                         {node("Assign", "",
                               {id("z"), binary("Plus", id("x"), id("y"))})})}),
              node("this")})});

    ExpectedNode const baz_method = node(
        "Method", "",
        {type_node("int"), id("baz"), node("Method body", "", {integer("0")})});

    ExpectedNode const foo_class =
        node("Class", "",
             {id("Foo"), node("Class body", "",
                              {foo_fields, node("Method declaration list", "",
                                                {bar_method, baz_method})})});

    ExpectedNode const empty_class =
        node("Class", "", {id("Empty"), node("Empty class body")});

    return node(
        "Class declaration list", "",
        {node("Main Class", "", {id("Main"), id("args"), main_statements}),
         foo_class, empty_class});
}

void assert_ast_eq(const Node *actual, const ExpectedNode &expected,
                   const std::string &path = "root") {
    ASSERT_NE(actual, nullptr) << "Actual node is null at " << path;
    SCOPED_TRACE(::testing::Message() << "AST path: " << path);
    ASSERT_EQ(actual->type, expected.type);
    EXPECT_EQ(actual->value, expected.value);
    ASSERT_EQ(actual->children.size(), expected.children.size());

    auto it = actual->children.begin();
    for (std::size_t i = 0; i < expected.children.size(); ++i, ++it) {
        assert_ast_eq(it->get(), expected.children[i],
                      path + "/" + expected.children[i].type + "[" +
                          std::to_string(i) + "]");
    }
}

parsing::Result<std::unique_ptr<Node>>
parse_source(std::string_view source, CollectingDiagnosticSink &diag) {
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &diag);
    parsing::Parser parser(std::move(lexer), &diag);
    return parser.parse_goal();
}

} // namespace

TEST(ParserExact, FullTree) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    a = 1 + 2 - 3 * 4 / 5;
    b = -1;
    flag = true;
    flag = false;
    if (!flag && a < b || a > b) {
    }
    if (a == b) {
      System.out.println(0);
    } else {
    }
    while (b < 10) {
      b = b + 1;
    }
    arr = new int[7];
    a = arr.length;
    b = arr[1];
    foo = new Foo();
    a = foo.bar(8, 9);
    a = foo.baz();
    System.out.println(a);
    arr[2] = 3;
  }
}

class Foo {
  int x;
  boolean flag;
  int[] arr;
  Foo other;

  public int bar(int x, int y) {
    int z;
    z = x + y;
    return this;
  }

  public int baz() {
    return 0;
  }
}

class Empty {
}
)";
    CollectingDiagnosticSink diag;
    auto parse_result = parse_source(source, diag);
    ASSERT_TRUE(parse_result.has_value());
    assert_no_errors(diag.diagnostics);

    ExpectedNode const expected = expected_tree();
    assert_ast_eq(parse_result.value().get(), expected);
}

TEST(ParserExact, ParseErrorKindExpectedTokenAssign) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    x 1;
  }
}
)";

    CollectingDiagnosticSink diag;
    auto parse_result = parse_source(source, diag);
    ASSERT_FALSE(parse_result.has_value());
    const parsing::ParseError &error = parse_result.error();
    EXPECT_EQ(error.kind, parsing::ParseErrorKind::ExpectedToken);
    ASSERT_TRUE(error.expected_token.has_value());
    EXPECT_EQ(error.expected_token.value(), lexing::TokenKind::Assign);
}

TEST(ParserExact, ParseErrorKindExpectedType) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    x = 1;
  }
}

class Foo {
  public ;
}
)";

    CollectingDiagnosticSink diag;
    auto parse_result = parse_source(source, diag);
    ASSERT_FALSE(parse_result.has_value());
    const parsing::ParseError &error = parse_result.error();
    EXPECT_EQ(error.kind, parsing::ParseErrorKind::ExpectedType);
    EXPECT_FALSE(error.expected_token.has_value());
}

TEST(ParserExact, ParseErrorKindExpectedExpression) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    x = ;
  }
}
)";

    CollectingDiagnosticSink diag;
    auto parse_result = parse_source(source, diag);
    ASSERT_FALSE(parse_result.has_value());
    const parsing::ParseError &error = parse_result.error();
    EXPECT_EQ(error.kind, parsing::ParseErrorKind::ExpectedExpression);
    EXPECT_FALSE(error.expected_token.has_value());
}

TEST(ParserExact, ParseErrorKindExpectedStatement) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    return 0;
  }
}
)";

    CollectingDiagnosticSink diag;
    auto parse_result = parse_source(source, diag);
    ASSERT_FALSE(parse_result.has_value());
    const parsing::ParseError &error = parse_result.error();
    EXPECT_EQ(error.kind, parsing::ParseErrorKind::ExpectedStatement);
    EXPECT_FALSE(error.expected_token.has_value());
}
