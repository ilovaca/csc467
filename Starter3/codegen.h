#ifndef CODE_GEN_H
#define CODE_GEN_H

#include "ast.h"
struct node_;
typedef struct node_ node;


void codegen();
void codegen(node*, int);

#endif