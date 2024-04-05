#include <iostream>

#include "BytecodeProgram.hpp"
#include "CFG.hpp"
#include "Node.h"
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
    if (!lexical_errors) {
        std::cerr << "Syntax errors found! See the logs below:" << std::endl;
        std::cerr << "\t@error at line " << yylineno
                  << ". Cannot generate a syntax for this input:" << err.c_str()
                  << std::endl;
        std::cerr << "End of syntax errors!" << std::endl;
        errCode = errCodes::SYNTAX_ERROR;
    }
}

void generateGraphviz(Node *root) {
    std::ofstream outStream;
    std::string filename{"tree.dot"};
    outStream.open(filename);

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

    generateGraphviz(root);

    CFG graph;

    std::fstream controlFlowGraph("cfg.dot", std::ios::out);
    root->generateIR(graph, st);
    graph.printGraphviz(controlFlowGraph);

    std::fstream stGraph("st.dot", std::ios::out);
    st.printTable(stGraph);

    BytecodeProgram program;
    graph.generateBytecode(program, st);
    std::fstream prettyBytecode("bytecode.txt", std::ios::out);
    program.print(prettyBytecode);

    std::ofstream bytecodeProgram("prog.bc", std::ios::binary);
    program.serialize(bytecodeProgram);

    return errCodes::SUCCESS;
}
