#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

#include "ast/Node.h"
#include "bytecode/BytecodeProgram.hpp"
#include "ir/CFG.hpp"
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

    if (!fs::exists(outputDirectoryName)) {
        if (!fs::create_directory(outputDirectoryName)) {
            std::cerr << "Failed to create directory '" << outputDirectoryName
                      << "'.\n";
            return 1;
        }
    }

    const auto &currentDirectory = std::filesystem::current_path();
    const auto &outputDirectory = currentDirectory / outputDirectoryName;

    // Reads from file if a file name is passed as an argument. Otherwise, reads
    // from stdin.
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
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
