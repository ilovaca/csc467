#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <map>
#include <stack>
#include "ast.h"

typedef std::map<std::string, type_code> SYBL_T;
// extern SYBL_T *current;
extern std::stack<SYBL_T> symbol_stack;

#endif

