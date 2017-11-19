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

    {"env1", {.type = VEC4,.predef = UNIFORM}},
    {"env2", {.type = VEC4,.predef = UNIFORM}},
    {"env3", {.type = VEC4,.predef = UNIFORM}},
};

std::vector<SYBL_T*> symbol_stack = {&predefinedST};

std::pair<std::string, struct symbol_attr> searchPredefined(const char* id) {
  std::pair<std::string, struct symbol_attr> ret("", {.type = ERROR, .predef = NONE});
  std::string key = id;
  auto it = symbol_stack.front()->find(key);
  if (it != symbol_stack.front()->end()){
      // found
      ret = *it;
      return ret;
  }
  return ret;
}


std::pair<std::string, struct symbol_attr> searchSymbolTable(const char* id) {
  std::pair<std::string, struct symbol_attr> ret("", {.type = ERROR, .predef = NONE});
  std::string key = id;
  for (auto rit = symbol_stack.rbegin(); rit != symbol_stack.rend(); ++rit) {
    auto it = (**rit).find(key);
    if (it != (**rit).end()){
      // found
      ret = *it;
      return ret;
    }
  }
  return ret;
}
