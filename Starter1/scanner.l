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
#include <errno.h>
#include <limits.h>


#define YY_USER_INIT { yyin = inputFile; }
#define yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }
// #define MYDEBUG 1
int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS            [ \t]
DIGIT         [0-9]
ALPHABET      [a-zA-Z]
ALPHANUMERIC  [a-zA-Z0-9]
%%
[" ""\t"]+  {}
["\n""\r"]  {
                yyline++;
            }
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
                    char *end = NULL;
                    errno = 0;
                    long temp = strtol(yytext, &end, 10);
                    if (errno != ERANGE && temp >= INT_MIN && temp <= INT_MAX){
                        yylval.intVal = (int)temp;
                    } else {
                        yERROR("out of bound integer");
                    }
                    #if MYDEBUG == 1
                    printf("[SCANNER] integer: %s, value: %d\n", yytext, yylval.intVal);
                    #endif
                    yTRACE(LITERAL_INT);
                    return LITERAL_INT;
                }

[+-]?{DIGIT}+"."{DIGIT}+    {
                                yylval.floatVal = atof(yytext);
                                #if MYDEBUG == 1
                                printf("[SCANNER] floating point: %s, value: %f\n", yytext, yylval.floatVal);
                                #endif
                                yTRACE(LITERAL_FLOAT);
                                return LITERAL_FLOAT;
                            }

"="             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_ASSIGNMENT);
                    return OPERATOR_ASSIGNMENT;
                }
"+"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_PLUS);
                    return OPERATOR_PLUS;
                }
"-"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_MINUS);
                    return OPERATOR_MINUS;
                }
"*"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_MULT);
                    return OPERATOR_MULT;
                }
"/"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_DIV);
                    return OPERATOR_DIV;
                }
"^"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_EXP);
                    return OPERATOR_EXP;
                }
"!"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_NOT);
                    return OPERATOR_NOT;
                }
"&&"            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_AND);
                    return OPERATOR_AND;
                }
"||"            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_OR);
                    return OPERATOR_OR;
                }
"=="            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_EQUAL);
                    return OPERATOR_EQUAL;
                }
"!="            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_NOT_EQUAL);
                    return OPERATOR_NOT_EQUAL;
                }
"<"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_LESS_THAN);
                    return OPERATOR_LESS_THAN;
                }
"<="            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_LESS_THAN_OR_EQUAL_TO);
                    return OPERATOR_LESS_THAN_OR_EQUAL_TO;
                }
">"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_LARGER_THAN);
                    return OPERATOR_LARGER_THAN;
                }
">="            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] operator:\"%s\"\n", yytext);
                    #endif
                    yTRACE(OPERATOR_LARGER_THAN_OR_EQUAL_TO);
                    return OPERATOR_LARGER_THAN_OR_EQUAL_TO;
                }

"true"          {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yylval.boolVal = true;
                    yTRACE(KEYWORD_TRUE);
                    return KEYWORD_TRUE;
                }
"false"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yylval.boolVal = false;
                    yTRACE(KEYWORD_FALSE);
                    return KEYWORD_FALSE;
                }
"const"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_QUALIFIER_CONST);
                    return KEYWORD_QUALIFIER_CONST;
                }
"if"            {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_IF);
                    return KEYWORD_IF;
                }
"else"          {   
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_ELSE);
                    return KEYWORD_ELSE;
                }
"while"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_WHILE);
                    return KEYWORD_WHILE;
                }
"lit"           {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_FUNCNAME_LIT);
                    return KEYWORD_FUNCNAME_LIT;
                }
"dp3"           {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_FUNCNAME_DP3);
                    return KEYWORD_FUNCNAME_DP3;
                }
"rsq"           {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_FUNCNAME_RSQ);
                    return KEYWORD_FUNCNAME_RSQ;
                }
"int"           {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_INT);
                    return KEYWORD_DATATYPE_INT;
                }
"float"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_FLOAT);
                    return KEYWORD_DATATYPE_FLOAT;
                }
"bool"          {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_BOOL);
                    return KEYWORD_DATATYPE_BOOL;
                }
"vec2"          {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_VEC2);
                    return KEYWORD_DATATYPE_VEC2;
                }
"vec3"          {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_VEC3);
                    return KEYWORD_DATATYPE_VEC3;
                }
"vec4"          {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_VEC4);
                    return KEYWORD_DATATYPE_VEC4;
                }
"bvec2"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_BVEC2);
                    return KEYWORD_DATATYPE_BVEC2;
                }
"bvec3"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_BVEC3);
                    return KEYWORD_DATATYPE_BVEC3;
                }
"bvec4"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_BVEC4);
                    return KEYWORD_DATATYPE_BVEC4;
                }
"ivec2"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_IVEC2);
                    return KEYWORD_DATATYPE_IVEC2;
                }
"ivec3"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_IVEC3);
                    return KEYWORD_DATATYPE_IVEC3;
                }
"ivec4"         {
                    #if MYDEBUG == 1
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    #endif
                    yTRACE(KEYWORD_DATATYPE_IVEC4);
                    return KEYWORD_DATATYPE_IVEC4;
                }
"{"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_LEFT_BRACE);
                    return CHAR_LEFT_BRACE;
                }
"}"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_RIGHT_BRACE);
                    return CHAR_RIGHT_BRACE;
                }
"["             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_LEFT_SQUARE_BRACKET);
                    return CHAR_LEFT_SQUARE_BRACKET;
                }
"]"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_RIGHT_SQUARE_BRACKET);
                    return CHAR_RIGHT_SQUARE_BRACKET;
                }
"("             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_LEFT_PARENTHESIS);
                    return CHAR_LEFT_PARENTHESIS;
                }
")"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_RIGHT_PARENTHESIS);
                    return CHAR_RIGHT_PARENTHESIS;
                }
";"             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_SEMI_COLON);
                    return CHAR_SEMI_COLON;
                }
","             {
                    #if MYDEBUG == 1
                    printf("[SCANNER] character:\"%s\"\n", yytext);
                    #endif
                    yTRACE(CHAR_COMMA);
                    return CHAR_COMMA;
                }
[0-9]+[a-z0-9"_"]*                  {
                                        yERROR("invalid identifier");
                                    }
[a-z]+[a-z0-9"_"]*                  {
                                        #if MYDEBUG == 1
                                        printf("[SCANNER] identifier:\"%s\"\n", yytext);
                                        #endif
                                        yylval.stringVal = strdup(yytext);
                                        yTRACE(LITERAL_IDENTIFIER);
                                        return LITERAL_IDENTIFIER;
                                    }
<<EOF>>         {   
                    yyterminate();
                }
.               {
                    printf("Unknown token: %s\n", yytext);
                    yERROR("Unknown token");
                }

%%