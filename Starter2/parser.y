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
// #define MYDEBUG 1
void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */
extern char *yytext;

enum {
  DP3 = 0, 
  LIT = 1, 
  RSQ = 2
};

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

// defines the yyval union
%union {
  int as_int;
  float as_float;
  char *as_str;
}

%token          FLOAT_T
%token          INT_T
%token          BOOL_T
%token          CONST
%token          FALSE_C TRUE_C
%token          FUNC_DQ3 FUNC_RSQ FUNC_LIT
%token          VEC2 VEC3 VEC4 IVEC2 IVEC3 IVEC4 BVEC2 BVEC3 BVEC4
%token          IF WHILE
%nonassoc       "then" /*resolving if else conflicts*/
%nonassoc       ELSE

// links specific values of tokens to yyval
%token <as_float> FLOAT_C
%token <as_int>   INT_C
%token <as_str>   ID

%left   OR
%left   AND
%left   EQ NEQ '<' LEQ '>' GEQ
%left   '+' '-'
%left   '*' '/'
%right  '^'
%left   '!' UMINUS
%left   '[' ']' '(' ')'


%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 ***********************************************************************/
program
    : scope                                                     { yTRACE("program -> scope"); }
    ;
scope
    : '{' declarations statements '}'                           { yTRACE("scope -> { declarations statements }"); }
    ;
declarations
    : declarations declaration                                  { yTRACE("declarations -> declarations declaration"); }
    | /*empty*/                                                 { yTRACE("declarations -> empty");}
    ;
statements
    : statements statement                                      { yTRACE("statements -> statements statement"); }
    | /*empty*/                                                 { yTRACE("statements -> empty"); }
    ;
declaration
    : type ID ';'                                               { yTRACE("declaration -> type ID ;"); }
    | type ID '=' expression ';'                                { yTRACE("declaration -> type ID = expression ;"); }
    | CONST type ID '=' expression ';'                          { yTRACE("CONST type ID -> expression ;"); }
    ;
statement
    : variable '=' expression ';'                               { yTRACE("statement -> variable = expression ;"); }
    | IF '(' expression ')' statement  %prec "then"             { yTRACE("statement -> IF (expression) statement"); }
    | IF '(' expression ')' statement  ELSE statement           { yTRACE("statement -> IF (expression) statement ELSE statement"); }
    | WHILE '(' expression ')' statement                        { yTRACE("statement -> WHILE(expression) statement"); }
    | scope                                                     { yTRACE("statement -> scope"); }
    | ';'                                                       { yTRACE("statement -> ;"); }
    ;

type
    : INT_T                                                     { yTRACE("type -> INT_T"); }
    | FLOAT_T                                                   { yTRACE("type -> FLOAT_T"); }
    | BOOL_T                                                    { yTRACE("type -> BOOL_T"); }
    | VEC2                                                      { yTRACE("type -> VEC2"); }
    | VEC3                                                      { yTRACE("type -> VEC3"); }
    | VEC4                                                      { yTRACE("type -> VEC4"); }
    | IVEC2                                                     { yTRACE("type -> IVEC2"); }
    | IVEC3                                                     { yTRACE("type -> IVEC3"); }
    | IVEC4                                                     { yTRACE("type -> IVEC4"); }
    | BVEC2                                                     { yTRACE("type -> BVEC2"); }
    | BVEC3                                                     { yTRACE("type -> BVEC3"); }
    | BVEC4                                                     { yTRACE("type -> BVEC4"); }
    ;
expression
    : constructor                                               { yTRACE("expression -> constructor"); }
    | function                                                  { yTRACE("expression -> function"); }
    | INT_C                                                     { yTRACE("expression -> INT_C"); }
    | FLOAT_C                                                   { yTRACE("expression -> FLOAT_C"); }
    | variable                                                  { yTRACE("expression -> variable"); }
    | '!' expression %prec UMINUS                               { yTRACE("expression -> ! expression"); }
    | '-' expression %prec UMINUS                               { yTRACE("expression -> - expression"); }
    | expression AND expression                                 { yTRACE("expression -> expression && expression"); }
    | expression OR  expression                                 { yTRACE("expression -> expression || expression"); }
    | expression EQ  expression                                 { yTRACE("expression -> expression == expression"); }
    | expression NEQ expression                                 { yTRACE("expression -> expression != expression"); }
    | expression '<' expression                                 { yTRACE("expression -> expression < expression"); }
    | expression LEQ expression                                 { yTRACE("expression -> expression <= expression"); }
    | expression '>' expression                                 { yTRACE("expression -> expression > expression"); }
    | expression GEQ expression                                 { yTRACE("expression -> expression >= expression"); }
    | expression '+' expression                                 { yTRACE("expression -> expression + expression"); }
    | expression '-' expression                                 { yTRACE("expression -> expression - expression"); }
    | expression '*' expression                                 { yTRACE("expression -> expression * expression"); }
    | expression '/' expression                                 { yTRACE("expression -> expression / expression"); }
    | expression '^' expression                                 { yTRACE("expression -> expression ^ expression"); }
    | TRUE_C                                                    { yTRACE("expression -> TRUE_C"); }
    | FALSE_C                                                   { yTRACE("expression -> FALSE_C"); }
    | '(' expression ')'                                        { yTRACE("expression -> (expression)"); }
    ;
variable
    : ID                                                        { yTRACE("variable -> ID"); }
    | ID '[' INT_C ']'                                          { yTRACE("variable -> ID[INT_C]"); }
    ;
constructor
    : type '(' arguments ')'                                    { yTRACE("constructor -> type (arguments)"); }
    ;
function
    : function_name '(' arguments_opt ')'                       { yTRACE("function -> function_name(arguments_opt)"); }
    ;
function_name
    : FUNC_LIT                                                  { yTRACE("function_name -> FUNC_LIT"); }
    | FUNC_RSQ                                                  { yTRACE("function_name -> FUNC_RSQ"); }
    | FUNC_DQ3                                                  { yTRACE("function_name -> FUNC_DQ3"); }
    ;
arguments_opt
    : arguments                                                 { yTRACE("arguments_opt -> arguments"); }
    | /*empty*/                                                 { yTRACE("arguments_opt -> empty"); }
    ;
arguments
    : arguments ',' expression                                  { yTRACE("arguments -> arguments , expression"); }
    | expression                                                { yTRACE("arguments -> expression"); }
    ;


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if(errorOccurred) {
    return;    /* Error has already been reported by scanner */
  } else {
    errorOccurred = 1;
  }

  fprintf(errorFile, "\nPARSER ERROR, LINE %d", yyline);
  
  if(strcmp(s, "parse error")) {
    if(strncmp(s, "parse error, ", 13)) {
      fprintf(errorFile, ": %s\n", s);
    } else {
      fprintf(errorFile, ": %s\n", s+13);
    }
  } else {
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
  }
}

