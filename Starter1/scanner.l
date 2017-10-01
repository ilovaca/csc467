%{
/**********************************************************************
 *
 * **YOUR GROUP INFO SHOULD GO HERE**
 *
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"
#include <stdlib.h>



#define YY_USER_INIT { yyin = inputFile; }
#define yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS            [ \t]
DIGIT         [0-9]
ALPHABET      [a-zA-Z]
ALPHANUMERIC  [a-zA-Z0-9]
%%
[" ""\n""\t""\r"]+ {}
"/*"        {
                int c1      = 0;
                int c2      = yyinput();
                int curline = yyline;
                for(;;) {
                    if (c2 == EOF) {
                        fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: Unmatched /*\n", curline);
                        errorOccurred = TRUE;
                        yyterminate();
                    }
                if (c1 == '*' && c2 == '/')
                    break;
                c1 = c2;
                c2 = yyinput();
                if (c1 == '\n' && c2 != EOF)
                    yyline++;
                }
            }
[+-]?{DIGIT}+   {
                    yylval.intVal = atoi(yytext);
                    printf("[SCANNER] integer: %s, value: %d\n", yytext, yylval.intVal);
                    return LITERAL_INT;
                }

[+-]?{DIGIT}+"."{DIGIT}+    {
                                yylval.floatVal = atof(yytext);
                                printf("[SCANNER] floating point: %s, value: %f\n", yytext, yylval.floatVal);
                                return LITERAL_FLOAT;
                            }

"="             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_ASSIGNMENT;
                }
"+"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_PLUS;
                }
"-"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_MINUS;
                }
"*"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_MULT;
                }
"/"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_DIV;
                }
"^"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_EXP;
                }
"!"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_NOT;
                }
"&&"            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_AND;
                }
"||"            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_OR;
                }
"=="            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_EQUAL;
                }
"!="            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_NOT_EQUAL;
                }
"<"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_LESS_THAN;
                }
"<="            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_LESS_THAN_OR_EQUAL_TO;
                }
">"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_LARGER_THAN;
                }
">="            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    return OPERATOR_LARGER_THAN_OR_EQUAL_TO;
                }


"true"          {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yylval.boolVal = true;
                    return KEYWORD_TRUE;
                }
"false"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yylval.boolVal = false;
                    return KEYWORD_FALSE;
                }
"const"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_QUALIFIER_CONST;
                }
"if"            {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_IF;
                }
"else"          {   
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_ELSE;
                }
"while"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_WHILE;
                }
"lit"           {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_FUNCNAME_LIT;
                }
"dp3"           {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_FUNCNAME_DP3;
                }
"rsq"           {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif

                    return KEYWORD_FUNCNAME_RSQ;
                }
"int"           {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_INT;
                }
"float"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_FLOAT;
                }
"bool"          {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_BOOL;
                }
"vec2"          {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_VEC2;
                }
"vec3"          {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_VEC3;
                }
"vec4"          {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_VEC4;
                }
"bvec2"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_BVEC2;
                }
"bvec3"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_BVEC3;
                }
"bvec4"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_BVEC4;
                }
"ivec2"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_IVEC2;
                }
"ivec3"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_IVEC3;
                }
"ivec4"         {
                    #if YYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    return KEYWORD_DATATYPE_IVEC4;
                }
"{"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_LEFT_BRACE;
                }
"}"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_RIGHT_BRACE;
                }
"["             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_LEFT_SQUARE_BRACKET;
                }
"]"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_RIGHT_SQUARE_BRACKET;
                }
"("             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_LEFT_PARENTHESIS;
                }
")"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_RIGHT_PARENTHESIS;
                }
";"             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_SEMI_COLON;
                }
","             {
                    #if YYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    return CHAR_COMMA;
                }
[a-z]+[a-z0-9"_"]*                  {
                                        #if YYDEBUG == 1
                                        printf("[SCANNER] identifier:\"%s\"\n", yytext);
                                        #endif
                                        yylval.stringVal = strdup(yytext);
                                        return LITERAL_IDENTIFIER;
                                    }
<<EOF>>         {   
                    printf("Done, exiting\n");
                    yyterminate();
                }
.               {
                    printf("Unknown token %s\n", yytext);
                    yERROR("Unknown token");
                }

%%