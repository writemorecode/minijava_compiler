CXX := g++
CXXFLAGS := -g -Wall -Wextra -Wpedantic -std=c++17

NODES := $(wildcard *Node.cpp)
NODES_OBJ := $(NODES:.cpp=.o)

compiler: lex.yy.c parser.tab.o main.cpp $(NODES_OBJ) SymbolTable.o Scope.o Record.o Variable.o Method.o Class.o
		$(CXX) $(CXXFLAGS) $^ -o $@
parser.tab.o: parser.tab.cc
		$(CXX) $(CXXFLAGS) $^ -c -o $@
parser.tab.cc: parser.yy
		bison parser.yy
lex.yy.c: lexer.flex parser.tab.cc
		flex lexer.flex

tree: 
	dot -Tpdf tree.dot -o tree.pdf
st: 
	dot -Tpdf st.dot -o st.pdf

clean:
		rm -f parser.tab.* lex.yy.c* compiler stack.hh position.hh location.hh *.dot *.pdf *.o 
	
