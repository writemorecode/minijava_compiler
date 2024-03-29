%skeleton "lalr1.cc"
%defines
%define parse.error verbose
%define api.value.type variant
%define api.token.constructor

%code requires{
  #include <string>
  #include "Node.h"
  #include "ArrayAccessNode.hpp"
  #include "ArrayLengthNode.hpp"
  #include "ExpressionNode.hpp"
  #include "ArithmeticExpressionNode.hpp"
  #include "BooleanExpressionNode.hpp"
  #include "BooleanNode.hpp"
  #include "ClassAllocationNode.hpp"
  #include "LogicalExpressionNode.hpp"
  #include "MethodNode.hpp"
  #include "MethodWithoutParametersNode.hpp"
  #include "MethodParameterNode.hpp"
  #include "MethodCallWithoutArgumentsNode.hpp"
  #include "MethodCallNode.hpp"
  #include "MethodBodyNode.hpp"
  #include "NotNode.hpp"
  #include "VariableNode.hpp"
  #include "ClassNode.hpp"
  #include "MainClassNode.hpp"
  #include "ControlStatementNode.hpp"
  #include "StatementNode.hpp"
  #include "IntegerNode.hpp"
  #include "IntegerArrayAllocationNode.hpp"
  #include "IdentifierNode.hpp"
  #include "TypeNode.hpp"
  #include "ThisNode.hpp"
  #define USE_LEX_ONLY false //change this macro to true if you want to isolate the lexer from the parser.
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
  YY_DECL;

  Node* root;
  extern int yylineno;

}

// definition of set of tokens. All tokens are of type string
%token <std::string> PLUSOP MINUSOP MULTOP DIVOP
%token <std::string> L_PAREN R_PAREN L_CURLY R_CURLY L_SQUARE R_SQUARE
%token <std::string> ASSIGN SEMI COMMA DOT NOT RETURN
%token <std::string> EQ AND OR LT GT TRUE FALSE LENGTH THIS
%token <std::string> PUBLIC STATIC VOID MAIN WHILE CLASS NEW IF ELSE PRINTLN STRING INT INT_LITERAL BOOL ID
%token END 0 "end of file"

//definition of operator precedence. See https://www.gnu.org/software/bison/manual/bison.html#Precedence-Decl
%right "then" ELSE

%right OR
%right AND
%left GT LT EQ NOT
%left PLUSOP MINUSOP
%left MULTOP DIVOP
%left L_PAREN L_SQUARE
%left DOT

// definition of the production rules. All production rules are of type Node
%type <Node *> Goal MainClass
%type <Node *> Statement StatementList
%type <Node *> MethodDeclaration MethodParameter MethodParameterList MethodBody MethodBodyItem MethodBodyItemList
%type <Node *> VarDeclaration
%type <Node *> ClassDeclaration ClassDeclarationList ClassBody
%type <Node *> ClassVarDeclarationList MethodDeclarationList
%type <Node *> Expression ExpressionList
%type <Node *> Type Identifier Integer

%%

/* A MiniJava source file consists of a main class, followed by zero or more other classes. */
Goal: MainClass END {
    root = $1;

};
| MainClass ClassDeclarationList END {
    $$ = $2;
    $$->children.push_front($1);
    root = $2;
}

MainClass: PUBLIC CLASS Identifier L_CURLY PUBLIC STATIC VOID MAIN L_PAREN STRING L_SQUARE R_SQUARE Identifier R_PAREN L_CURLY StatementList R_CURLY  R_CURLY {
	$$ = new MainClassNode($3, $13, $16, $3->lineno);
};

/* A class is declared using the "class" keyword, followed by an identifier and a class body. */
ClassDeclaration: CLASS Identifier ClassBody {
	$$ = new ClassNode($2, $3, $2->lineno);
};

/*
  The body of a class can:
  * be empty,
  * consist of one or more variable declarations,
  * consist of one or more method declarations,
  * or consist one or more variable declarations, followed by one or more method declarations.
*/
ClassBody: L_CURLY R_CURLY {
    $$ = new Node("Empty class body", "", yylineno);
};
| L_CURLY ClassVarDeclarationList R_CURLY {
    $$ = $2;
};
| L_CURLY MethodDeclarationList R_CURLY {
    $$ = $2;
};
| L_CURLY ClassVarDeclarationList MethodDeclarationList R_CURLY {
    $$ = new Node("Class body", "", yylineno);
    $$->children.push_back($2);
    $$->children.push_back($3);
};

/* A ClassVarDeclarationList consists of one or more variables that are declared inside of a class body */
ClassVarDeclarationList: VarDeclaration {
    $$ = new Node("Variable declaration list", "", yylineno);
    $$->children.push_back($1);
};
| ClassVarDeclarationList VarDeclaration {
    $$ = $1;
    $$->children.push_back($2);
};

/* A MethodDeclarationList consists of one or more methods that are declared inside of a class */
MethodDeclarationList: MethodDeclaration {
    $$ = new Node("Method declaration list", "", yylineno);
    $$->children.push_back($1);
};
| MethodDeclarationList MethodDeclaration {
    $$ = $1;
    $$->children.push_back($2);
};

/* A ClassDeclarationList consists of one or more class declarations */
ClassDeclarationList: ClassDeclaration {
    $$ = new Node("Class declaration list", "", yylineno);
    $$->children.push_back($1);
};
| ClassDeclarationList ClassDeclaration {
    $$ = $1;
    $$->children.push_back($2);
};

Statement: L_CURLY R_CURLY {
	$$ = new Node("Empty statement", yylineno);
};
| L_CURLY StatementList R_CURLY {
    $$ = $2;
};
| IF L_PAREN Expression R_PAREN Statement %prec "then" {
	$$ = new IfNode($3, $5, yylineno);
};
| IF L_PAREN Expression R_PAREN Statement ELSE Statement {
	$$ = new IfElseNode($3, $5, $7, yylineno);
};
| WHILE L_PAREN Expression R_PAREN Statement {
	$$ = new WhileNode($3, $5, yylineno);
};
| PRINTLN L_PAREN Expression R_PAREN SEMI {
    $$ = new PrintNode($3, yylineno);
};
| Identifier ASSIGN Expression SEMI {
    $$ = new AssignNode($1, $3, yylineno);
};
| Identifier L_SQUARE Expression R_SQUARE ASSIGN Expression SEMI {
    $$ = new ArrayAssignNode($1, $3, $6, yylineno);
};

StatementList: StatementList Statement {
    $$ = $1;
    $$->children.push_back($2);
};
| Statement {
    $$ = new Node("Statement list", "", yylineno);
    $$->children.push_back($1);
};

Expression: Expression PLUSOP Expression {
    $$ = new PlusNode($1,$3,yylineno);
};
| Expression MINUSOP Expression {
    $$ = new MinusNode($1,$3,yylineno);
};
| Expression MULTOP Expression {
    $$ = new MultiplicationNode($1,$3,yylineno);
};
| Expression DIVOP Expression {
    $$ = new DivisionNode($1,$3,yylineno);
};
| Expression OR Expression {
	$$ = new OrNode($1, $3, yylineno);
};
| Expression AND Expression {
	$$ = new AndNode($1, $3, yylineno);
};
| Expression LT Expression {
	$$ = new LessThanNode($1,$3,yylineno);
};
| Expression GT Expression {
	$$ = new GreaterThanNode($1,$3,yylineno);
};
| Expression EQ Expression {
	$$ = new EqualToNode($1,$3,yylineno);
};
| Expression L_SQUARE Expression R_SQUARE {
	$$ = new ArrayAccessNode($1, $3, yylineno);
};
| Expression DOT LENGTH {
    $$ = new ArrayLengthNode($1, yylineno);
};
| Expression DOT Identifier L_PAREN R_PAREN {
	$$ = new MethodCallWithoutArgumentsNode($1, $3, yylineno);
};
| Expression DOT Identifier L_PAREN ExpressionList R_PAREN {
	$$ = new MethodCallNode($1, $3, $5, yylineno);
};
| Integer { $$ = $1; };
| TRUE {
    $$ = new TrueNode(yylineno);
};
| FALSE {
    $$ = new FalseNode(yylineno);
};
| Identifier { $$ = $1; };
| THIS {
    $$ = new ThisNode(yylineno);
};
| NEW INT L_SQUARE Expression R_SQUARE {
    $$ = new IntegerArrayAllocationNode($4, yylineno);
};
| NEW Identifier L_PAREN R_PAREN {
	$$ = new ClassAllocationNode($2, yylineno);
};
| NOT Expression {
    $$ = new NotNode($2,yylineno);
};
| L_PAREN Expression R_PAREN { $$ = $2; };

ExpressionList: ExpressionList COMMA Expression {
    $$ = $1;
    $$->children.push_back($3);
};
| Expression {
    $$ = new Node("Expression list", "", yylineno);
    $$->children.push_back($1);
};

Type: INT L_SQUARE R_SQUARE {
    $$ = new TypeNode("int[]", yylineno);
};
| BOOL {
    $$ = new TypeNode("boolean", yylineno);
};
| INT {
    $$ = new TypeNode("int", yylineno);
};
| Identifier {
    $$ = new TypeNode($1->value, yylineno);
};

VarDeclaration: Type Identifier SEMI {
	$$ = new VariableNode($1, $2, yylineno);
};


MethodDeclaration: PUBLIC Type Identifier L_PAREN R_PAREN L_CURLY MethodBody R_CURLY {
	$$ = new MethodWithoutParametersNode($2, $3, $7, $2->lineno);
};
| PUBLIC Type Identifier L_PAREN MethodParameterList R_PAREN L_CURLY MethodBody R_CURLY {
	$$ = new MethodNode($2, $3, $5, $8, $2->lineno);
};

/* A MethodBodyItem can be either a variable declaration or a method declaration, that is located inside of a method body.  */
MethodBodyItem: VarDeclaration {
    $$ = new Node("Variable declaration", "", yylineno);
    $$->children.push_back($1);
};
| Statement {
    $$ = new Node("Statement", "", yylineno);
    $$->children.push_back($1);
};

MethodBodyItemList: MethodBodyItem {
    $$ = new Node("Method body item list", "", yylineno);
    $$->children.push_back($1);
};
| MethodBodyItemList MethodBodyItem {
    $$ = $1;
    $1->children.push_back($2);
};

MethodBody: RETURN Expression SEMI {
	$$ = new ReturnOnlyMethodBodyNode($2, yylineno);
};
| MethodBodyItemList RETURN Expression SEMI {
	$$ = new MethodBodyNode($1, $3, yylineno);
};

/* A MethodParameter is a parameter that is passed to a method, e.g. "int x" for int foo(int x) { return x; } */
MethodParameter: Type Identifier {
	$$ = new MethodParameterNode($1, $2, yylineno);
};

/* A MethodParameterList is a list of parameters that is passed to a method, e.g. "int x, bool y" for int foo(int x, bool y) { return x; } */
MethodParameterList: MethodParameter {
    $$ = new Node("Method parameter list", "", yylineno);
    $$->children.push_back($1);
};
| MethodParameterList COMMA MethodParameter {
    $$ = $1;
    $$->children.push_back($3);
};

Identifier: ID {
    $$ = new IdentifierNode($1, yylineno);
};
Integer: INT_LITERAL {
    $$ = new IntegerNode($1, yylineno);
};
| MINUSOP INT_LITERAL {
    $$ = new IdentifierNode("-$2", yylineno);
};
