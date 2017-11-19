#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include "symbol.h"
#include "ast.h"
using namespace std;

SYBL_T predefinedST = {
    {"gl_FragColor", {.type = VEC4,.predef = RESULT}},
    {"gl_FragDepth", {.type = BOOL,.predef = RESULT}},
    {"gl_FragCoord", {.type = VEC4,.predef = RESULT}},

    {"gl_TexCoord",     {.type = VEC4,.predef = ATTRIBUTE}},
    {"gl_Color",        {.type = VEC4,.predef = ATTRIBUTE}},
    {"gl_Secondary",    {.type = VEC4,.predef = ATTRIBUTE}},
    {"gl_FogFragCoord", {.type = VEC4,.predef = ATTRIBUTE}},

    {"gl_Light_Half",           {.type = VEC4,.predef = UNIFORM}},
    {"gl_Light_Ambient",        {.type = VEC4,.predef = UNIFORM}},
    {"gl_Material_Shininess",   {.type = VEC4,.predef = UNIFORM}},

    {"gl_env1", {.type = VEC4,.predef = UNIFORM}},
    {"gl_env2", {.type = VEC4,.predef = UNIFORM}},
    {"gl_env3", {.type = VEC4,.predef = UNIFORM}},
};

std::vector<SYBL_T*> symbol_stack = {&predefinedST};

// void addPredefinedSymbolTable() {
//     SYBL_T* temp = new SYBL_T;
//     struct symbol_attr s {.type = VEC4,
//                             .predef = RESULT};
//     // temp->insert(std::pair<std::string, struct symbol_attr>("gl_FragColor", s));
//     temp->insert(std::pair<std::string, struct symbol_attr>("gl_FragColor", {.type = VEC4,
//                             .predef = RESULT}));
//     symbol_stack.push_back(temp);
// }

// traverse the tree and create symbol table 
// for each declaration in the scope
void buildSymbolTable(node * n) {
  if (n == NULL) return;
  node_kind kind = n->kind;
  switch(kind) {
    case SCOPE_NODE:
      {
        // entering a scope, create symbol table
        n->scope.symbol_table = new SYBL_T;
        // then push this table to the stack, as current scope
        symbol_stack.push_back(n->scope.symbol_table);
        // descent to build and create nested symbol tables
        if (n->scope.declarations) {
          buildSymbolTable(n->scope.declarations);
        }
        // statements can contain scope
        if (n->scope.statements) {
          buildSymbolTable(n->scope.statements);
        }
        // leaving current scope, pop it from stack
        symbol_stack.pop_back();
        break;
      }
    case DECLARATIONS_NODE:
      {
        // descent to print each DECLARATION
        if (n->declarations_node.left)
          buildSymbolTable(n->declarations_node.left);
        if (n->declarations_node.right)
          buildSymbolTable(n->declarations_node.right);
        break;
      }
    case DECLARATION_NODE:
      {
        // insert variable name and type to current symbol table
        // cout << n->declaration_node.ident << " " 
        //     << type_name[n->declaration_node.kids[0]->type_node.code];
        auto current = symbol_stack.back();
        // TODO: if this ID has already been declared, report error.
        auto ret = current->insert(std::pair<std::string, struct symbol_attr>(
                            string(n->declaration_node.ident),
                            {.type = (type_code)n->declaration_node.kids[0]->type_node.code,
                                .predef = n->declaration_node.type == 2? CONST_VAR : (predef_attr) 0}));
        if (!ret.second) {
            SEMANTIC_ERROR("ERROR: redefining identifier");
        }
        // don't need to traverse further, avoid printing the expression node again
        break;
      }

    case STATEMENTS_NODE:
      {
        // need to traverse all statements and grab the names?
        if (n->statements_node.left)
          buildSymbolTable(n->statements_node.left);
        if (n->statements_node.right)
          buildSymbolTable(n->statements_node.right);
        break;
      }
    case IF_STATEMENT_NODE:
      {
        // condition won't create symbol tables
        // Then statement can create new scope
        buildSymbolTable(n->if_stmt_node.kids[1]);
        // print the else statement if exists
        if (n->if_stmt_node.withElse) {
          buildSymbolTable(n->if_stmt_node.kids[2]);
        }
        break;
      }
    default: break;
  }
}

type_code searchSymbolTable(const char* id) {
  type_code ret = ERROR;
  std::string key = id;
  for (auto rit = symbol_stack.rbegin(); rit != symbol_stack.rend(); ++rit) {
    auto it = (**rit).find(key);
    if (it != (**rit).end()){
      // found
      ret = it->second.type;
      return ret;
    }
  }
  return ret;
}
