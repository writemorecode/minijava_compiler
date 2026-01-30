#include <filesystem>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

#include "ast/Node.h"
#include "bytecode/BytecodeProgram.hpp"
#include "ir/CFG.hpp"
#include "lexing/Lexer.hpp"
#include "lexing/LegacyDiagnostics.hpp"
#include "lexing/SourceBuffer.hpp"
#include "lexing/StringViewStream.hpp"
#include "parser.tab.hh"

extern Node *root;
extern FILE *yyin;
extern int yylineno;
extern int lexical_errors;
extern yy::parser::symbol_type yylex();

enum errCodes {
    SUCCESS = 0,
    LEXICAL_ERROR = 1,
    SYNTAX_ERROR = 2,
    AST_ERROR = 3,
    SEMANTIC_ERROR = 4,
    SEGMENTATION_FAULT = 139
};

int errCode = errCodes::SUCCESS;

void print_legacy_token(const lexing::Token &token) {
    using lexing::TokenKind;
    switch (token.kind) {
    case TokenKind::Plus:
        std::cout << "PLUSOP\n";
        return;
    case TokenKind::Minus:
        std::cout << "MINUSOP\n";
        return;
    case TokenKind::Star:
        std::cout << "MULTOP\n";
        return;
    case TokenKind::Slash:
        std::cout << "DIVOP\n";
        return;
    case TokenKind::LParen:
        std::cout << "L_PAREN\n";
        return;
    case TokenKind::RParen:
        std::cout << "R_PAREN\n";
        return;
    case TokenKind::LCurly:
        std::cout << "L_CURLY\n";
        return;
    case TokenKind::RCurly:
        std::cout << "R_CURLY\n";
        return;
    case TokenKind::LSquare:
        std::cout << "L_SQUARE\n";
        return;
    case TokenKind::RSquare:
        std::cout << "R_SQUARE\n";
        return;
    case TokenKind::Semi:
        std::cout << "SEMI\n";
        return;
    case TokenKind::Lt:
        std::cout << "LT\n";
        return;
    case TokenKind::Gt:
        std::cout << "GT\n";
        return;
    case TokenKind::AndAnd:
        std::cout << "AND\n";
        return;
    case TokenKind::OrOr:
        std::cout << "OR\n";
        return;
    case TokenKind::EqEq:
        std::cout << "EQ\n";
        return;
    case TokenKind::Bang:
        std::cout << "NOT\n";
        return;
    case TokenKind::Dot:
        std::cout << "DOT\n";
        return;
    case TokenKind::Assign:
        std::cout << "ASSIGN\n";
        return;
    case TokenKind::Comma:
        std::cout << "COMMA\n";
        return;
    case TokenKind::KwPublic:
        std::cout << "PUBLIC\n";
        return;
    case TokenKind::KwStatic:
        std::cout << "STATIC\n";
        return;
    case TokenKind::KwVoid:
        std::cout << "VOID\n";
        return;
    case TokenKind::KwMain:
        std::cout << "MAIN\n";
        return;
    case TokenKind::KwString:
        std::cout << "STRING\n";
        return;
    case TokenKind::KwInt:
        std::cout << "INT\n";
        return;
    case TokenKind::KwBoolean:
        std::cout << "BOOL\n";
        return;
    case TokenKind::KwIf:
        std::cout << "IF\n";
        return;
    case TokenKind::KwElse:
        std::cout << "ELSE\n";
        return;
    case TokenKind::KwWhile:
        std::cout << "WHILE\n";
        return;
    case TokenKind::KwPrintln:
        std::cout << "PRINTLN\n";
        return;
    case TokenKind::KwLength:
        std::cout << "LENGTH\n";
        return;
    case TokenKind::KwTrue:
        std::cout << "TRUE\n";
        return;
    case TokenKind::KwFalse:
        std::cout << "FALSE\n";
        return;
    case TokenKind::KwThis:
        std::cout << "THIS\n";
        return;
    case TokenKind::KwNew:
        std::cout << "NEW\n";
        return;
    case TokenKind::KwReturn:
        std::cout << "RETURN\n";
        return;
    case TokenKind::KwClass:
        std::cout << "CLASS\n";
        return;
    case TokenKind::IntLiteral:
        std::cout << "INT LITERAL (" << token.lexeme << ")\n";
        return;
    case TokenKind::Identifier:
        std::cout << "STR LITERAL '" << token.lexeme << "'\n";
        return;
    case TokenKind::Eof:
    case TokenKind::Invalid:
        return;
    }
}

// Handling Syntax Errors
void yy::parser::error(std::string const &err) {
    if (lexical_errors == 0) {
        std::cerr << "Syntax errors found! See the logs below:\n";
        std::cerr << "\t@error at line " << yylineno
                  << ". Cannot generate a syntax for this input:" << err
                  << "\n";
        std::cerr << "End of syntax errors!\n";
        errCode = errCodes::SYNTAX_ERROR;
    }
}

void generateGraphviz(Node *root, std::ofstream &outStream) {
    int count = 0;
    outStream << "digraph {" << '\n';
    root->printGraphviz(count, outStream);
    outStream << "}" << '\n';
    outStream.close();
}

void printTree(const Node *root) {
    int depth = 0;
    root->print(depth);
}

int main(int argc, char **argv) {
    const std::string outputDirectoryName = "output";
    bool lex_only = false;
    std::string input_path;

    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--lex" || arg == "-lex") {
            lex_only = true;
            continue;
        }
        if (input_path.empty()) {
            input_path = argv[i];
        }
    }

    if (!fs::exists(outputDirectoryName)) {
        if (!fs::create_directory(outputDirectoryName)) {
            std::cerr << "Failed to create directory '" << outputDirectoryName
                      << "'.\n";
            return 1;
        }
    }

    const auto &currentDirectory = std::filesystem::current_path();
    const auto &outputDirectory = currentDirectory / outputDirectoryName;

    if (lex_only) {
        lexical_errors = 0;
        std::string error;
        lexing::SourceBuffer buffer =
            lexing::SourceBuffer::from_string(std::string{});

        if (!input_path.empty()) {
            auto loaded = lexing::SourceBuffer::from_file(input_path, error);
            if (!loaded.has_value()) {
                std::cerr << error << "\n";
                return 1;
            }
            buffer = std::move(*loaded);
        } else {
            std::string data((std::istreambuf_iterator<char>(std::cin)),
                             std::istreambuf_iterator<char>());
            buffer = lexing::SourceBuffer::from_string(std::move(data));
        }

        auto stream =
            std::make_unique<lexing::StringViewStream>(buffer.view());
        lexing::LegacyDiagnosticSink diag(&lexical_errors);
        lexing::Lexer lexer(std::move(stream), buffer.view(), &diag);

        while (true) {
            lexing::Token token = lexer.next();
            if (token.kind == lexing::TokenKind::Eof) {
                break;
            }
            if (token.kind == lexing::TokenKind::Invalid) {
                continue;
            }
            print_legacy_token(token);
        }

        return lexical_errors ? errCodes::LEXICAL_ERROR : errCodes::SUCCESS;
    }

    // Reads from file if a file name is passed as an argument. Otherwise, reads
    // from stdin.
    if (!input_path.empty()) {
        if (!(yyin = fopen(input_path.c_str(), "r"))) {
            perror(input_path.c_str());
            return 1;
        }
    }
    if (USE_LEX_ONLY) {
        yylex();
        return errCodes::SUCCESS;
    }

    yy::parser parser;
    bool parseSuccess = !parser.parse();
    if (!parseSuccess) {
        return errCodes::SYNTAX_ERROR;
    }

    bool lexSuccess = !lexical_errors;
    if (!lexSuccess) {
        return errCodes::LEXICAL_ERROR;
    }

    SymbolTable st;
    bool symbolTableSuccess = root->buildTable(st);
    if (!symbolTableSuccess) {
        std::cout << "Symbol table construction failed.\n";
    }

    auto rootType = root->checkTypes(st);
    bool typeCheckSuccess = !rootType.empty();
    if (!typeCheckSuccess) {
        std::cout << "Type checking failed.\n";
    }

    if (!symbolTableSuccess || !typeCheckSuccess) {
        return errCode;
    }

    std::ofstream outStream(outputDirectory / "tree.dot");
    generateGraphviz(root, outStream);

    CFG graph;
    std::ofstream controlFlowGraph(outputDirectory / "cfg.dot");

    if (!controlFlowGraph.is_open()) {
        std::cerr << "Failed to open file cfg.dot.\n";
        return 1;
    }

    root->generateIR(graph, st);
    graph.printGraphviz(controlFlowGraph);

    std::ofstream stGraph(outputDirectory / "st.dot");
    st.printTable(stGraph);

    BytecodeProgram program;
    graph.generateBytecode(program, st);

    std::ofstream prettyBytecode(outputDirectory / "bytecode.txt");
    program.print(prettyBytecode);

    std::ofstream bytecodeProgram(outputDirectory / "prog.bc");
    program.serialize(bytecodeProgram);

    return errCodes::SUCCESS;
}
