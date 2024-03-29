CXX := g++
CXXFLAGS := -g -Wall -Wextra -Wpedantic -std=c++20

NODES := $(wildcard *Node.cpp)
NODES_OBJ := $(NODES:.cpp=.o)

TACS := $(wildcard *Tac.cpp)
TACS_OBJ := $(TACS:.cpp=.o)

BYTECODES := $(wildcard Bytecode*.cpp)
BYTECODES_OBJ := $(BYTECODES:.cpp=.o)

compiler: lex.yy.o parser.tab.o main.cpp $(NODES_OBJ) $(TACS_OBJ) $(BYTECODES_OBJ) SymbolTable.o Scope.o Record.o Variable.o Method.o Class.o Tac.o BBlock.o CFG.o
		$(CXX) $(CXXFLAGS) $^ -o $@
parser.tab.o: parser.tab.cc
		$(CXX) $(CXXFLAGS) $^ -c -o $@
parser.tab.cc: parser.yy
		bison parser.yy
lex.yy.c: lexer.flex parser.tab.cc
		flex lexer.flex
lex.yy.o: lex.yy.c
		$(CXX) $(CXXFLAGS) $^ -c -o $@

tree: 
	dot -Tpdf tree.dot -o tree.pdf
st: 
	dot -Tpdf st.dot -o st.pdf

clean:
		rm -f parser.tab.* lex.yy.c* compiler stack.hh position.hh location.hh *.dot *.pdf *.o 
		rm -rf compiler.dSYM
	
