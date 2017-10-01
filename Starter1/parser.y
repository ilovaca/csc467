%{
/***********************************************************************
 * --YOUR GROUP INFO SHOULD GO HERE--
 * 
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
  int   intVal;
  float floatVal;
  bool  boolVal;
  char* stringVal;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
// Literals
%token           	myToken1 myToken2
%token <num>        LITERAL_INT
%token <floatVal>   LITERAL_FLOAT
%token <stringVal>  LITERAL_IDENTIFIER
// Operators
%token OPERATOR_ASSIGNMENT
%token OPERATOR_PLUS
%token OPERATOR_MINUS
%token OPERATOR_MULT
%token OPERATOR_DIV
%token OPERATOR_EXP
%token OPERATOR_NOT
%token OPERATOR_AND
%token OPERATOR_OR
%token OPERATOR_EQUAL
%token OPERATOR_NOT_EQUAL
%token OPERATOR_LESS_THAN
%token OPERATOR_LESS_THAN_OR_EQUAL_TO
%token OPERATOR_LARGER_THAN
%token OPERATOR_LARGER_THAN_OR_EQUAL_TO
// KEYWORDS
%token KEYWORD_TRUE
%token KEYWORD_FALSE
%token KEYWORD_IF
%token KEYWORD_ELSE
%token KEYWORD_WHILE
%token KEYWORD_FUNCNAME_LIT
%token KEYWORD_FUNCNAME_DP3
%token KEYWORD_FUNCNAME_RSQ
%token KEYWORD_QUALIFIER_CONST

%token KEYWORD_DATATYPE_INT
%token KEYWORD_DATATYPE_BOOL
%token KEYWORD_DATATYPE_FLOAT
%token KEYWORD_DATATYPE_VEC2
%token KEYWORD_DATATYPE_VEC3
%token KEYWORD_DATATYPE_VEC4
%token KEYWORD_DATATYPE_BVEC2
%token KEYWORD_DATATYPE_BVEC3
%token KEYWORD_DATATYPE_BVEC4
%token KEYWORD_DATATYPE_IVEC2
%token KEYWORD_DATATYPE_IVEC3
%token KEYWORD_DATATYPE_IVEC4


// other characters
%token CHAR_LEFT_BRACE
%token CHAR_RIGHT_BRACE
%token CHAR_LEFT_SQUARE_BRACKET
%token CHAR_RIGHT_SQUARE_BRACKET
%token CHAR_LEFT_PARENTHESIS
%token CHAR_RIGHT_PARENTHESIS
%token CHAR_SEMI_COLON
%token CHAR_COLON
%token CHAR_COMMA

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :   tokens       
  ;
tokens
  :  tokens token
  |
  ;
// TODO: replace myToken with the token the you defined.
token
  :     myToken1
  |     myToken2
  |     LITERAL_INT     { printf("[PARSER] Found INT %d\n", yylval.intVal);}
  |     LITERAL_FLOAT   { printf("[PARSER] Found FLOAT %f\n", yylval.floatVal);}
  |     LITERAL_IDENTIFIER { printf("[PARSER] Found IDENTIFIER %s\n", yylval.stringVal);free(yylval.stringVal);}
  ;


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

