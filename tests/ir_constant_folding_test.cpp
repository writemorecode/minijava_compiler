#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "ast/Node.h"
#include "bytecode/BytecodeInstruction.hpp"
#include "bytecode/BytecodeProgram.hpp"
#include "bytecode/Opcode.hpp"
#include "ir/CFG.hpp"
#include "ir/IRGenerationVisitor.hpp"
#include "ir/passes/ConditionalJumpFoldingPass.hpp"
#include "ir/passes/ConstantFoldingPass.hpp"
#include "ir/passes/IRPassManager.hpp"
#include "lexing/Diagnostics.hpp"
#include "lexing/Lexer.hpp"
#include "lexing/StringViewStream.hpp"
#include "parsing/Parser.hpp"
#include "semantic/SymbolTable.hpp"
#include "semantic/SymbolTableVisitor.hpp"
#include "semantic/TypeCheckVisitor.hpp"
#include "util/serialize.hpp"

namespace {

class CollectingDiagnosticSink final : public lexing::DiagnosticSink {
  public:
    void emit(lexing::Diagnostic d) override { diagnostics.push_back(d); }

    [[nodiscard]] int error_count() const {
        int count = 0;
        for (const auto &d : diagnostics) {
            if (d.severity == lexing::Severity::Error) {
                count += 1;
            }
        }
        return count;
    }

    std::vector<lexing::Diagnostic> diagnostics;
};

std::unique_ptr<BytecodeProgram>
compile_with_constant_folding(std::string_view source) {
    CollectingDiagnosticSink parser_diag;
    auto stream = std::make_unique<lexing::StringViewStream>(source);
    lexing::Lexer lexer(std::move(stream), source, &parser_diag);
    parsing::Parser parser(std::move(lexer), &parser_diag);
    auto parse_result = parser.parse_goal();

    EXPECT_TRUE(parse_result.has_value());
    EXPECT_EQ(parser_diag.error_count(), 0);
    if (!parse_result.has_value() || parser_diag.error_count() != 0) {
        return nullptr;
    }

    auto root = std::move(parse_result.value());

    SymbolTable symbol_table;
    CollectingDiagnosticSink semantic_diag;
    const auto symbol_table_result =
        build_symbol_table(*root, symbol_table, &semantic_diag);
    EXPECT_TRUE(symbol_table_result.ok());

    TypeInfo type_info;
    const auto type_check_result =
        check_types(*root, symbol_table, &type_info, &semantic_diag);
    EXPECT_TRUE(type_check_result.ok());
    EXPECT_EQ(semantic_diag.error_count(), 0);
    if (!symbol_table_result.ok() || !type_check_result.ok() ||
        semantic_diag.error_count() != 0) {
        return nullptr;
    }

    CFG graph;
    graph.setTypeInfo(&type_info);
    const auto ir_result =
        generate_ir(*root, graph, symbol_table, &semantic_diag);
    EXPECT_TRUE(ir_result.ok());
    EXPECT_EQ(semantic_diag.error_count(), 0);
    if (!ir_result.ok() || semantic_diag.error_count() != 0) {
        return nullptr;
    }

    IRPassManager pass_manager;
    pass_manager.addPass(std::make_unique<ConstantFoldingPass>());
    pass_manager.addPass(std::make_unique<ConditionalJumpFoldingPass>());
    (void)pass_manager.run(graph);

    auto program = std::make_unique<BytecodeProgram>();
    graph.generateBytecode(*program, symbol_table);
    return program;
}

[[nodiscard]] std::vector<const BytecodeInstruction *>
collect_instructions(const BytecodeProgram &program) {
    return program.getInstructions();
}

[[nodiscard]] bool contains_instruction(
    const std::vector<const BytecodeInstruction *> &instructions,
    Opcode opcode) {
    for (const auto *instruction : instructions) {
        if (instruction->getOpcode() == opcode) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool
contains_const(const std::vector<const BytecodeInstruction *> &instructions,
               std::int64_t value) {
    for (const auto *instruction : instructions) {
        const auto *constant =
            dynamic_cast<const IntegerParameterInstruction *>(instruction);
        if (constant == nullptr) {
            continue;
        }
        if (constant->getOpcode() == Opcode::CONST &&
            constant->getParam() == value) {
            return true;
        }
    }
    return false;
}

} // namespace

TEST(IRConstantFolding, ArithmeticFoldRemovesAddAndMultiply) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    x = 2 + 3 * 4;
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_TRUE(contains_const(instructions, 14));
    EXPECT_FALSE(contains_instruction(instructions, Opcode::ADD));
    EXPECT_FALSE(contains_instruction(instructions, Opcode::MUL));
}

TEST(IRConstantFolding, ComparisonAndBooleanExpressionFoldsToConstant) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    if (new Foo().run()) {
      System.out.println(1);
    } else {
      System.out.println(0);
    }
  }
}

class Foo {
  public boolean run() {
    boolean x;
    x = (1 < 2) && (3 == 3);
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_TRUE(contains_const(instructions, 1));
    EXPECT_FALSE(contains_instruction(instructions, Opcode::LT));
    EXPECT_FALSE(contains_instruction(instructions, Opcode::EQ));
    EXPECT_FALSE(contains_instruction(instructions, Opcode::AND));
}

TEST(IRConstantFolding, NegativeConstantFoldProducesSignedIconst) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    x = 0 - 1;
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_TRUE(contains_const(instructions, -1));
}

TEST(IRConstantFolding, DivisionByZeroIsNotFolded) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    x = 4 / 0;
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_TRUE(contains_instruction(instructions, Opcode::DIV));
}

TEST(IRConstantFolding,
     IfElseConstantTrue_RemovesCjmp_ElseBecomesUnreachable) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    if (true) {
      x = 111;
    } else {
      x = 222;
    }
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_FALSE(contains_instruction(instructions, Opcode::CJMP));
    EXPECT_TRUE(contains_const(instructions, 111));
    EXPECT_FALSE(contains_const(instructions, 222));
}

TEST(IRConstantFolding,
     IfElseConstantFalse_RemovesCjmp_ThenBecomesUnreachable) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    if (false) {
      x = 333;
    } else {
      x = 444;
    }
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_FALSE(contains_instruction(instructions, Opcode::CJMP));
    EXPECT_FALSE(contains_const(instructions, 333));
    EXPECT_TRUE(contains_const(instructions, 444));
}

TEST(IRConstantFolding, WhileConstantFalse_RemovesCjmp_BodyBecomesUnreachable) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    x = 5;
    while (false) {
      x = 777;
    }
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_FALSE(contains_instruction(instructions, Opcode::CJMP));
    EXPECT_TRUE(contains_const(instructions, 5));
    EXPECT_FALSE(contains_const(instructions, 777));
}

TEST(IRConstantFolding, NonConstantCondition_KeepsCjmp) {
    constexpr std::string_view source =
        R"(public class Main {
  public static void main(String[] args) {
    System.out.println(new Foo().run());
  }
}

class Foo {
  public int run() {
    int x;
    x = 1;
    while (x < 3) {
      x = x + 1;
    }
    return x;
  }
}
)";

    const auto program = compile_with_constant_folding(source);
    ASSERT_NE(program, nullptr);
    const auto instructions = collect_instructions(*program);
    EXPECT_TRUE(contains_instruction(instructions, Opcode::CJMP));
}

TEST(IRConstantFolding, SignedSerializerRoundTrip) {
    const auto unique_suffix = std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto path = std::filesystem::temp_directory_path() /
                      ("minijava_signed_serializer_" + unique_suffix + ".bc");

    {
        std::ofstream stream(path, std::ios::binary);
        ASSERT_TRUE(stream.is_open());
        Serializer serializer(stream);
        serializer.writeSignedInteger(-1);
        serializer.writeSignedInteger(0);
        serializer.writeSignedInteger(42);
    }

    {
        std::ifstream stream(path, std::ios::binary);
        ASSERT_TRUE(stream.is_open());
        Deserializer deserializer(stream);
        EXPECT_EQ(deserializer.readSignedInteger(), -1);
        EXPECT_EQ(deserializer.readSignedInteger(), 0);
        EXPECT_EQ(deserializer.readSignedInteger(), 42);
    }

    std::error_code error;
    std::filesystem::remove(path, error);
    EXPECT_FALSE(error);
}
