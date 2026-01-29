CXX := g++
CXXFLAGS := -g -Wall -Wextra -Wpedantic -std=c++20 -Isrc -Ibuild/gen

SRC_DIR := src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
GEN_DIR := $(BUILD_DIR)/gen

BISON := /opt/homebrew/opt/bison/bin/bison
FLEX := flex

SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
VM_SOURCE := $(SRC_DIR)/vm/vm.cpp
COMPILER_SOURCES := $(filter-out $(VM_SOURCE),$(SOURCES))

COMPILER_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(COMPILER_SOURCES))
VM_OBJECTS := $(OBJ_DIR)/vm/vm.o $(OBJ_DIR)/util/serialize.o

PARSER_SRC := $(GEN_DIR)/parser.tab.cc
PARSER_HDR := $(GEN_DIR)/parser.tab.hh
LEX_SRC := $(GEN_DIR)/lex.yy.c
GEN_OBJECTS := $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o

FORMAT_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp' -o -name '*.hpp' -o -name '*.h')

all: vm compiler

compiler: $(BIN_DIR)/compiler
vm: $(BIN_DIR)/vm

$(BIN_DIR)/compiler: $(COMPILER_OBJECTS) $(GEN_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BIN_DIR)/vm: $(VM_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(COMPILER_OBJECTS): $(PARSER_HDR)

$(OBJ_DIR)/parser.tab.o: $(PARSER_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(OBJ_DIR)/lex.yy.o: $(LEX_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(PARSER_SRC) $(PARSER_HDR): $(SRC_DIR)/parsing/parser.yy
	@mkdir -p $(GEN_DIR)
	$(BISON) -d -o $(PARSER_SRC) $<

$(LEX_SRC): $(SRC_DIR)/lexing/lexer.flex $(PARSER_HDR)
	@mkdir -p $(GEN_DIR)
	$(FLEX) -o $(LEX_SRC) $<

tree: 
	dot -Tpdf output/tree.dot -o tree.pdf
st: 
	dot -Tpdf output/st.dot -o st.pdf
cfg: 
	dot -Tpdf output/cfg.dot -o cfg.pdf

fmt:
	clang-format -i $(FORMAT_SOURCES)

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.dot *.pdf
