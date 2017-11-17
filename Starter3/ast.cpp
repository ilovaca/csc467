#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <map>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0

void ast_traverse(node * n, bool postorder = true, void (*fp)(node * n) = NULL);
std::string getType(node *var_node) {
  return "";
}
const char* type_name[] = {
  "int",
  "ivec2",
  "ivec3",
  "ivec4",
  "bool",
  "bvec2",
  "bvec3",
  "bvec4",
  "float",
  "vec2",
  "vec3",
  "vec4"
};

const char* operator_name[] = {
    "&&",
    "||",
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "+",
    "-",
    "*",
    "/",
    "^"
};

const char* func_name[] = {
  "dp3",
  "lit",
  "rsq"
};

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast);
  ast->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  case SCOPE_NODE:
    {
      ast->scope.declarations = va_arg(args, node*);
      ast->scope.statements = va_arg(args, node*);
      break;
    }
  case DECLARATIONS_NODE:
  {
    ast->declarations_node.left = va_arg(args, node*);
    ast->declarations_node.right = va_arg(args, node*);
    break;
  }
  case DECLARATION_NODE:
    {
      // first extrat type
      ast->declaration_node.type = va_arg(args, int);
      // second the ID
      char* temp = va_arg(args, char*);
      int len = strlen(temp);
      ast->declaration_node.ident = (char *) calloc(len + 1, sizeof(char));
      memcpy(ast->declaration_node.ident, temp, len);
      // get type and expressin (if any)
      if (ast->declaration_node.type == 0) {
        // have only type subtree...
        ast->declaration_node.kids[0] = va_arg(args, node*);
        ast->declaration_node.kids[1] = NULL;
      } else if (ast->declaration_node.type == 1 
              || ast->declaration_node.type == 2) {
        // (CONST) type ID '=' expression
        // has 2 children: type and expression
        ast->declaration_node.kids[0] = va_arg(args, node*);
        ast->declaration_node.kids[1] = va_arg(args, node*);
      } else {
        assert(ast->declaration_node.type <= 2);
      }
      break;
    }
  case TYPE_NODE:
    {
      // type node does not have sub-nodes
      ast->type_node.code = va_arg(args, int);
      break;
    }
  case BINARY_EXPRESSION_NODE:
    { 
      ast->binary_expr.op = va_arg(args, int);
      ast->binary_expr.left = va_arg(args, node *);
      ast->binary_expr.right = va_arg(args, node *);
      break;
    }
  case LITERAL_NODE:
    {
      ast->literal.type = va_arg(args, int);
      if (ast->literal.type == 0) {
        // int literal
        ast->literal.ival = va_arg(args, int);
      } else if (ast->literal.type == 1) {
        // float literal
        ast->literal.fval = (float)va_arg(args, double);
      } else if (ast->literal.type == 2) {
        // bool literal
        ast->literal.bval = false;
      } else if (ast->literal.type == 3){
        ast->literal.bval = true;
      } else {
        assert(ast->literal.type <= 2);
      }
      break;
    }
  case VAR_NODE:
    {
      ast->var_node.type = va_arg(args, int);
      // get ID
      char* temp = va_arg(args, char*);
      int len = strlen(temp);
      ast->var_node.ident = (char *) calloc(len + 1, sizeof(char));
      memcpy(ast->var_node.ident, temp, len);
      // get index if any
      assert(ast->var_node.type <= 1);
      if (ast->var_node.type == 0) {
        ast->var_node.index = 0;
      } else {
        ast->var_node.index = va_arg(args, int);
      }
      break;
    }
  case FUNCTION_NODE:
    {
      ast->function_node.type = va_arg(args, int);
      ast->function_node.args = va_arg(args, node*);
      break;
    } 
  case CONSTRUCTOR_NODE:
    {
      ast->constructor_node.type = va_arg(args, node*);
      ast->constructor_node.arguments = va_arg(args, node*);
      break;
    }
  case STATEMENTS_NODE:
    {
      ast->statements_node.left = va_arg(args, node*);
      ast->statements_node.right = va_arg(args, node*);
      break;
    }
  case ASSIGNMENT_NODE:
    {
      ast->assignment_node.left = va_arg(args, node*);
      ast->assignment_node.right = va_arg(args, node*);
      break;
    }
  case IF_STATEMENT_NODE:
    {
      ast->if_stmt_node.withElse = (bool)va_arg(args, int);
      ast->if_stmt_node.kids[0] = va_arg(args, node*);
      ast->if_stmt_node.kids[1] = va_arg(args, node*);
      if (ast->if_stmt_node.withElse) {
        // if no ELSE statement, this node has only two kids: the condition and statement
        ast->if_stmt_node.kids[2] = va_arg(args, node*);
      } else {
        ast->if_stmt_node.kids[2] = NULL;
      }
      break;
    }
  case ARGUMENTS_NODE:
    {
      ast->arguments_node.left = va_arg(args, node*);
      ast->arguments_node.right = va_arg(args, node*);
      break;
    }
  case UNARY_EXPRESION_NODE:
    {
      ast->unary_expr.op = va_arg(args, int);
      ast->unary_expr.expr = va_arg(args, node*);
      break;
    }
  default: break;
  }

  va_end(args);

  return ast;
}

// this assumes that all kids have been freed
void deleter(node * ast) {
  assert(ast != NULL);
  // assert(ast->numkids == 0);
  // for (int i = 0; i < ast->numkids; i++) {
  //   free(ast->kids[i]);
  //   ast->kids[i] = NULL;
  // }
  free(ast);
}

void ast_free(node *n) {
}

void ast_print(node *n, int indent) {
  using namespace std;
  if (n == NULL) return;
  node_kind kind = n->kind;
  switch(kind) {
    case SCOPE_NODE:
      {
        cout << "(SCOPE)" << endl;
        // descent to print subtrees
        if (n->scope.declarations) {
          cout << setw(4) << ' ' << "(DECLARATIONS ...)" << endl;
          ast_print(n->scope.declarations, indent + 4);
        }
        if (n->scope.statements) {
          cout << setw(4) << ' ' << "(STATEMENTS ...)" << endl;
          ast_print(n->scope.statements, indent + 4);
          // cout << ")";
        }
        break;
      }
    case DECLARATIONS_NODE:
      {
        // cout << setw(indent) << ' ' <<"(DECLARATIONS ...)" << endl;
        // the ... may need to expand here.
        // descent to print each DECLARATION
        if (n->declarations_node.left)
          ast_print(n->declarations_node.left, indent + 4);
        if (n->declarations_node.right)
          ast_print(n->declarations_node.right, indent + 4);
        // cout << setw(indent) << ' ' << ")" << endl;
        break;
      }
    case DECLARATION_NODE:
      {
        cout << setw(indent) << ' ' <<"(DECLARATION ";
        // variable and type names
        cout << n->declaration_node.ident << " " 
            << type_name[n->declaration_node.kids[0]->type_node.code];
        if (n->declaration_node.type == 1
              || n->declaration_node.type == 2) {
          // print initial value in expression form.
          ast_print(n->declaration_node.kids[1], indent + 4);
        }
        cout << ")" <<endl;
        // don't need to traverse further, avoid printing the expression node again
        break;
      }
    case TYPE_NODE:
      {
        // leaf node
        // should never reach this node
        assert(false);
        cout << type_name[n->type_node.code] << endl;
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        cout << /*endl << setw(indent) << ' ' <<*/ " (BINARY ";
        // type must be the result type of the expression
        // FIXME: using the left expr as the type for now.
        cout << getType(n->binary_expr.left) << " ";
        cout << operator_name[n->binary_expr.op] << " ";
        // print left and right expr
        if (n->binary_expr.left)
          ast_print(n->binary_expr.left, indent + 4);
        if (n->binary_expr.right){
          ast_print(n->binary_expr.right, indent + 4);
          cout << ")";
        }
        break;
      }
    case LITERAL_NODE:
      {
        // LITERAL node is a leaf
        if (n->literal.type == 0) {
            cout << " <" << n->literal.ival << ">" /*<< endl*/;
        } else if (n->literal.type == 1){
            cout << " <" << n->literal.fval << ">" /*<< endl*/;

        } else {
            cout << std::boolalpha;
            cout << " <" << n->literal.bval << ">" /*<< endl*/;            
        }
        break;
      }
    case VAR_NODE:
      {
        // VAR_NODE is a leaf
        if (n->var_node.type == 0) {
          // regular variable
          cout << n->var_node.ident /* << endl*/;
        } else {
          // vector variable with index
          cout << setw(indent) << ' ' << "(INDEX ";
          // type of this variable
          cout << getType(n);
          cout << n->var_node.ident << " " << n->var_node.index << ")";
        }
        break;
      }
    case FUNCTION_NODE:
      {
        cout << "(CALL ";
        cout << func_name[n->function_node.type];
        // print arguments
        if (n->function_node.args)
          ast_print(n->function_node.args);
        cout << ")";
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        cout << "(CALL ";
        cout << type_name[n->constructor_node.type->type_node.code] << " ";
        // print arguments
        ast_print(n->constructor_node.arguments);
        break;
      }
    case STATEMENTS_NODE:
      {
        // need to traverse all statements and grab the names?
        if (n->statements_node.left)
          ast_print(n->statements_node.left, indent);
        if (n->statements_node.right)
          ast_print(n->statements_node.right, indent + 4);
        break;
      }
    case ASSIGNMENT_NODE:
      {
        cout << setw(indent) << ' ' << "(ASSIGN ";
        // type of variable, can get from the symbol table
        cout << getType(n->assignment_node.left);
        // variable name
        cout << n->assignment_node.left->var_node.ident << " ";
        // then print the expression node
        ast_print(n->assignment_node.right, indent + 4);
        cout << ")" << endl;
        // FIXME: the above statemetn has printed the assigning
        // expression already. Do I need to traverse the subtree
        // further, so that the expression itself is printed again.
        // I guess yes
        // if (n->assignment_node.left)
        //   ast_print(n->assignment_node.left);
        // if (n->assignment_node.right)
        //   ast_print(n->assignment_node.right);
        break;
      }
    case IF_STATEMENT_NODE:
      {
        cout << setw(indent)<< ' ' << "(IF ";
        // condition in expression form
        ast_print(n->if_stmt_node.kids[0], indent + 4);
        // print then_statement
        cout << endl;
        ast_print(n->if_stmt_node.kids[1], indent + 4);
        // print the else statement if exists
        if (n->if_stmt_node.withElse) {
          assert (n->if_stmt_node.kids[2] != NULL);
          cout << setw(indent)<< ' ' << " ELSE";
          ast_print(n->if_stmt_node.kids[2], 0);
        }
        cout << setw(indent)<< ' ' << ")" << endl;
        // // print subtree
        // if (n->if_stmt_node.withElse) {
        //   ast_print(n->if_stmt_node.kids[0]);
        //   ast_print(n->if_stmt_node.kids[1]);
        //   ast_print(n->if_stmt_node.kids[2]);
        // } else {
        //   ast_print(n->if_stmt_node.kids[0]);
        //   ast_print(n->if_stmt_node.kids[1]);
        // }
        break;
      }
    case ARGUMENTS_NODE:
      {
        // FIXME: this order may need to be reversed?
        // print left first then print right
        if (n->arguments_node.left)
          ast_print(n->arguments_node.left);
        if (n->arguments_node.right)
          ast_print(n->arguments_node.right);
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        cout << "(UNARY ";
        // type of the resulting expression
        cout << getType(n->unary_expr.expr) << " ";
        if (n->unary_expr.op == 0) {
            cout << "- ";
        }else {
            cout << "! ";
        }
        // print expression
        if (n->unary_expr.expr)
          ast_print(n->unary_expr.expr);
        cout << ")";
        break;
      }
    default: break;
  }
}

void ast_traverse(node * n, bool postorder, void (*fp)(node * n)) {
  if (n == NULL) {
    return;
  }
  node_kind kind = n->kind;
  if (!postorder) {
    if (fp)
      fp(n);
  }
  // do post order traversal depending on the type of current node
  switch(kind) {
    case SCOPE_NODE:
      {
        ast_traverse(n->scope.declarations);
        ast_traverse(n->scope.statements);
        break;
      }
    case DECLARATIONS_NODE:
      {
        if (n->declarations_node.left)
          ast_traverse(n->declarations_node.left);
        if (n->declarations_node.right)
          ast_traverse(n->declarations_node.right);
        break;
      }
    case DECLARATION_NODE:
      {
        if (n->declaration_node.type == 0) {
          if (n->declaration_node.kids[0]) {
            ast_traverse(n->declaration_node.kids[0]);
          }
        } else if (n->declaration_node.type == 1 
              || n->declaration_node.type == 2) {
          if (n->declaration_node.kids[0]) {
            ast_traverse(n->declaration_node.kids[0]);
          }
          if (n->declaration_node.kids[1]) {
            ast_traverse(n->declaration_node.kids[1]);
          }
        }
        break;
      }
    case TYPE_NODE:
      {
        // leaf node
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        if (n->binary_expr.left)
          ast_traverse(n->binary_expr.left);
        if (n->binary_expr.right)
          ast_traverse(n->binary_expr.right);
        break;
      }
    case LITERAL_NODE:
      {
        // LITERAL node is a leaf
        break;
      }
    case VAR_NODE:
      {
        // VAR_NODE is a leaf
        break;
      }
    case FUNCTION_NODE:
      {
        if (n->function_node.args)
          ast_traverse(n->function_node.args);
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        if (n->constructor_node.type)
          ast_traverse(n->constructor_node.type);
        if (n->constructor_node.arguments)
          ast_traverse(n->constructor_node.arguments);
        break;
      }
    case STATEMENTS_NODE:
      {
        if (n->statements_node.left)
          ast_traverse(n->statements_node.left);
        if (n->statements_node.right)
          ast_traverse(n->statements_node.right);
        break;
      }
    case ASSIGNMENT_NODE:
      {
        if (n->assignment_node.left)
          ast_traverse(n->assignment_node.left);
        if (n->assignment_node.right)
          ast_traverse(n->assignment_node.right);
        break;
      }
    case IF_STATEMENT_NODE:
      {
        if (n->if_stmt_node.withElse) {
          ast_traverse(n->if_stmt_node.kids[0]);
          ast_traverse(n->if_stmt_node.kids[1]);
          ast_traverse(n->if_stmt_node.kids[2]);
        } else {
          ast_traverse(n->if_stmt_node.kids[0]);
          ast_traverse(n->if_stmt_node.kids[1]);
        }
        break;
      }
    case ARGUMENTS_NODE:
      {
        if (n->arguments_node.left)
          ast_traverse(n->arguments_node.left);
        if (n->arguments_node.right)
          ast_traverse(n->arguments_node.right);
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        if (n->unary_expr.expr)
          ast_traverse(n->unary_expr.expr);
        break;
      }
    default: break;
  }
  if (postorder){
    if (fp)
      fp(n);
  }
}

void ast_postorder(node * ast, void (*f)(node * n)) {
  ast_traverse(ast, true, f);
}

void ast_preorder(node * ast, void (*f)(node * n)) {
  ast_traverse(ast, false, f);
}
