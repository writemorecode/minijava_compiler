%top{
#include "parser.tab.hh"
#define YY_DECL yy::parser::symbol_type yylex()
#include "ast/Node.h"
int lexical_errors = 0;
int lineno = 1;
void print_token(char *t);
void error();
}
%option yylineno noyywrap nounput batch noinput stack

INT_LITERAL 0|[1-9][0-9]*
ID [a-zA-Z][a-zA-Z0-9_]*
COMMENT \/\/.*

%%

"+"    { if(USE_LEX_ONLY) { printf("PLUSOP\n"); } else { return yy::parser::make_PLUSOP(yytext); } }
"-"    { if(USE_LEX_ONLY) { printf("MINUSOP\n"); } else { return yy::parser::make_MINUSOP(yytext); } }
"*"    { if(USE_LEX_ONLY) { printf("MULTOP\n"); } else { return yy::parser::make_MULTOP(yytext); } }
"/"    { if(USE_LEX_ONLY) { printf("DIVOP\n"); } else { return yy::parser::make_DIVOP(yytext); } }
"("    { if(USE_LEX_ONLY) { printf("L_PAREN\n"); } else { return yy::parser::make_L_PAREN(yytext); } }
")"    { if(USE_LEX_ONLY) { printf("R_PAREN\n"); } else { return yy::parser::make_R_PAREN(yytext); } }
"{"    { if(USE_LEX_ONLY) { printf("L_CURLY\n"); } else { return yy::parser::make_L_CURLY(yytext); } }
"}"    { if(USE_LEX_ONLY) { printf("R_CURLY\n"); } else { return yy::parser::make_R_CURLY(yytext); } }
"["    { if(USE_LEX_ONLY) { printf("L_SQUARE\n"); } else { return yy::parser::make_L_SQUARE(yytext); } }
"]"    { if(USE_LEX_ONLY) { printf("R_SQUARE\n"); } else { return yy::parser::make_R_SQUARE(yytext); } }
";"    { if(USE_LEX_ONLY) { printf("SEMI\n"); } else { return yy::parser::make_SEMI(yytext); } }
"<"    { if(USE_LEX_ONLY) { printf("LT\n"); } else { return yy::parser::make_LT(yytext); } }
">"    { if(USE_LEX_ONLY) { printf("GT\n"); } else { return yy::parser::make_GT(yytext); } }
"&&"    { if(USE_LEX_ONLY) { printf("AND\n"); } else { return yy::parser::make_AND(yytext); } }
"||"    { if(USE_LEX_ONLY) { printf("OR\n"); } else { return yy::parser::make_OR(yytext); } }
"=="    { if(USE_LEX_ONLY) { printf("EQ\n"); } else { return yy::parser::make_EQ(yytext); } }
"!"    { if(USE_LEX_ONLY) { printf("NOT\n"); } else { return yy::parser::make_NOT(yytext); } }
"."    { if(USE_LEX_ONLY) { printf("DOT\n"); } else { return yy::parser::make_DOT(yytext); } }
"="    { if(USE_LEX_ONLY) { printf("ASSIGN\n"); } else { return yy::parser::make_ASSIGN(yytext); } }
","    { if(USE_LEX_ONLY) { printf("COMMA\n"); } else { return yy::parser::make_COMMA(yytext); } }
"public"    { if(USE_LEX_ONLY) { printf("PUBLIC\n"); } else { return yy::parser::make_PUBLIC(yytext); } }
"static"    { if(USE_LEX_ONLY) { printf("STATIC\n"); } else { return yy::parser::make_STATIC(yytext); } }
"void"    { if(USE_LEX_ONLY) { printf("VOID\n"); } else { return yy::parser::make_VOID(yytext); } }
"main"    { if(USE_LEX_ONLY) { printf("MAIN\n"); } else { return yy::parser::make_MAIN(yytext); } }
"String"    { if(USE_LEX_ONLY) { printf("STRING\n"); } else { return yy::parser::make_STRING(yytext); } }
"int"    { if(USE_LEX_ONLY) { printf("INT\n"); } else { return yy::parser::make_INT(yytext); } }
"boolean"    { if(USE_LEX_ONLY) { printf("BOOL\n"); } else { return yy::parser::make_BOOL(yytext); } }
"if"    { if(USE_LEX_ONLY) { printf("IF\n"); } else { return yy::parser::make_IF(yytext); } }
"else"    { if(USE_LEX_ONLY) { printf("ELSE\n"); } else { return yy::parser::make_ELSE(yytext); } }
"while"    { if(USE_LEX_ONLY) { printf("WHILE\n"); } else { return yy::parser::make_WHILE(yytext); } }
"System.out.println"    { if(USE_LEX_ONLY) { printf("PRINTLN\n"); } else { return yy::parser::make_PRINTLN(yytext); } }
"length"    { if(USE_LEX_ONLY) { printf("LENGTH\n"); } else { return yy::parser::make_LENGTH(yytext); } }
"true"    { if(USE_LEX_ONLY) { printf("TRUE\n"); } else { return yy::parser::make_TRUE(yytext); } }
"false"    { if(USE_LEX_ONLY) { printf("FALSE\n"); } else { return yy::parser::make_FALSE(yytext); } }
"this"    { if(USE_LEX_ONLY) { printf("THIS\n"); } else { return yy::parser::make_THIS(yytext); } }
"new"    { if(USE_LEX_ONLY) { printf("NEW\n"); } else { return yy::parser::make_NEW(yytext); } }
"return"    { if(USE_LEX_ONLY) { printf("RETURN\n"); } else { return yy::parser::make_RETURN(yytext); } }
"class"    { if(USE_LEX_ONLY) { printf("CLASS\n"); } else { return yy::parser::make_CLASS(yytext); } }

{INT_LITERAL} { if(USE_LEX_ONLY) { printf("INT LITERAL (%s)\n", yytext); } else { return yy::parser::make_INT_LITERAL(yytext); } }
{ID} { if(USE_LEX_ONLY) { printf("STR LITERAL '%s'\n", yytext); } else { return yy::parser::make_ID(yytext); } }
{COMMENT} {}
[ \t\n]+ {}
. { fprintf(stderr, "    Line %d: lexical ('%s' symbol is not recognized by the grammar)\n", yylineno, yytext); lexical_errors = 1; }
%%
