#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <vector>
// #include "ast.h"
struct node_;
typedef struct node_ node;
typedef enum{
  INT  = 0,
  IVECT2 = 1,
  IVECT3 = 2,
  IVECT4 = 3,
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

typedef std::map<std::string, type_code> SYBL_T;

extern std::vector<SYBL_T*> symbol_stack;

void buildSymbolTable(node *);
#endif

