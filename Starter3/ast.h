
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <string>
// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union. If you choose to use C++, then I suggest looking
// into inheritance.

// forward declare
struct node_;
typedef struct node_ node;
extern node *ast;
extern std::string type_name[];
/*
typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  
  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5), 
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  VAR_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),

  STATEMENT_NODE        = (1 << 1),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),

  DECLARATION_NODE      = (1 << 15)
} node_kind;*/


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
  VEC4 = 11
} type_code;

typedef enum {
  UNKNOWN           ,
  PROGRAM_ROOT      ,
  SCOPE_NODE        ,
  DECLARATIONS_NODE ,
  DECLARATION_NODE  ,
  STATEMENTS_NODE   ,
  STATEMENT_NODE    ,
  EXPRESSION_NODE   ,
  UNARY_EXPRESION_NODE  ,
  BINARY_EXPRESSION_NODE,
  TYPE_NODE           ,
  INT_NODE            , 
  FLOAT_NODE          ,
  BOOL_NODE           ,
  VAR_NODE            ,
  FUNCTION_NODE       ,
  CONSTRUCTOR_NODE    ,
  LITERAL_NODE        ,
  IF_STATEMENT_NODE    ,
  WHILE_STATEMENT_NODE ,
  ASSIGNMENT_NODE      ,
  NESTED_SCOPE_NODE    ,
  ARGUMENTS_NODE
} node_kind;

struct node_ {

  // an example of tagging each node with a type
  node_kind kind;

  union {
    struct {
      node* declarations;
      node* statements;
    } scope;
    struct {
      node* left;
      node* right;
    } declarations_node;
    struct {
      node* left;
      node* right;
    } statements_node;
    struct {
      node* left; // variable node
      node* right; // expression
    } assignment_node;
    struct {
      bool withElse;
      node *kids[3];
    } if_stmt_node;
    struct {
      int type;
      node* kids[2]; // type node and expression node
      char* ident; // identifier of variable
    } declaration_node;

    struct {
      int op;
      node *expr; // !/- expr
    } unary_expr;

    struct {
      int op;
      node *left; // more expressions...
      node *right; // current expression
    } binary_expr;
    // data types
    struct {
      int code; // encoding of data types
    } type_node;
    struct {
      char * ident;
    } id_node;
    struct {
      int type;
      int ival;
      float fval;
      bool bval;
    } literal;
    struct {
      int type;
      char * ident;
      int index;
    } var_node;
    struct {
      int type;
      node* args;
    } function_node;
    struct {
      node* type;
      node* arguments;
    } constructor_node;
    struct {
      node* left; // more args...
      node* right; // current arg...
    } arguments_node;
  };
};


node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast, int indent = 0);
void ast_preorder(node * ast, void (*f)(node * n));
void ast_postorder(node * ast, void (*f)(node * n));

#endif /* AST_H_ */