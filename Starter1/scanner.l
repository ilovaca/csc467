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
{DIGIT}+        {
                    printf("[SCANNER] unsigned integer: %s, length: %d\n", yytext, yyleng);
                    yylval.num = atoi(yytext);
                    return INT;
                }

{DIGIT}+"."{DIGIT}+ {
                      printf("[SCANNER] unsigned floating point %s\n", yytext);
                      yylval.floatVal = atof(yytext);
                      return FLOAT;
                    }
"foobar"        {
                    printf("[SCANNER] keyword:\"%s\"\n", yytext);
                    yylval.keyword = strdup(yytext);
                    return KEYWORD;
                }
{ALPHANUMERIC}+    {
                    printf("[SCANNER] found string \"%s\"\n", yytext);
                    yylval.stringVal = strdup(yytext);
                    return STRING;
                }
.           { yERROR("Unknown token"); }

%%