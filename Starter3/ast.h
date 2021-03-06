
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <string>
#include "symbol.h"
#include <vector>
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
  int row;
  int col;
  union {
    struct {
      node* declarations;
      node* statements;
      SYBL_T * symbol_table;
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
      std::string cond;
    } if_stmt_node;
    struct {
      int type; // 0 is non-initialized, 1 is with initializing expresion
      node* kids[2]; // type node and expression node
      char* ident; // identifier of variable
    } declaration_node;

    struct {
      int op;
      node *expr; // !/- expr
      type_code result_type;
      std::string reg_name;
    } unary_expr;

    struct {
      int op;
      node *left; // more expressions...
      node *right; // current expression
      type_code result_type;
      std::string reg_name;
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
      std::string reg_name;
    } literal;
    struct {
      int type;
      char *ident;
      int index;
      type_code result_type;
      predef_attr attr;
      std::string reg_name;
    } var_node;
    struct {
      int type; // name of the function
      node* args; // arguments
      type_code result_type;
      std::string reg_name;
    } function_node;
    struct {
      node* type; //type_node
      node* arguments;
      type_code result_type;
      // std::vector<std::string> reg_name;
      std::string reg_name;
    } constructor_node;
    struct {
      node* left; // more args...
      node* right; // current arg...
      int num_args;
      std::vector<type_code> args_type;
      std::vector<int> reg_ids;
      std::vector<std::string> reg_name;
    } arguments_node;
  };
};


node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast, int indent = 0);


#endif /* AST_H_ */
