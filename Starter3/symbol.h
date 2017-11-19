#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <vector>
#include <iostream>
#include "common.h"
struct node_;
typedef struct node_ node;
extern int yyline;
extern int yycolumn;
extern node* cur_node;
#define SEMANTIC_ERROR(x) { errorOccurred = 1; fprintf(traceFile, "[LINE %d, COL %d] %s\n", cur_node->row, cur_node->col, x); }

typedef enum{
  INT  = 0,
  IVEC2 = 1,
  IVEC3 = 2,
  IVEC4 = 3,
  BOOL = 4,
  BVEC2 = 5,
  BVEC3 = 6,
  BVEC4 = 7,
  FLOAT = 8,
  VEC2 = 9,
  VEC3 = 10,
  VEC4 = 11,
  ERROR = 12
} type_code;

typedef enum {
    NONE = 0,
    ATTRIBUTE,
    UNIFORM,
    RESULT,
    CONST_VAR
} predef_attr;

struct symbol_attr
{
    type_code type;
    predef_attr predef;
    symbol_attr(type_code t, predef_attr p):type(t), predef(p) {}
};

typedef std::map<std::string, struct symbol_attr> SYBL_T;

extern std::vector<SYBL_T*> symbol_stack;
extern SYBL_T predefined_vars;
// void addPredefinedSymbolTable();
void buildSymbolTable(node *);
std::pair<std::string, struct symbol_attr> searchPredefined(const char* id);
std::pair<std::string, struct symbol_attr> searchSymbolTable(const char* id);
#endif

