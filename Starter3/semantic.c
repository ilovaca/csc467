
#include "semantic.h"
#include "common.h"
#include <iostream>
#include <cassert>
using namespace std;

extern const char* operator_name[];

bool isVector(type_code T) {
  if (T == VEC2 || T == VEC3 || T == VEC4
      || T == IVEC2 || T == IVEC3 || T == IVEC4
      || T == BVEC2 || T == BVEC3 || T == BVEC4){
    return true;
  }
  else return false;
}

int typeDimension(type_code T) {
    if (!isVector(T))
        return 1;
    else {
        if (T == VEC2 || T == IVEC2 || T == BVEC2){
            return 2;
        } else if (T == VEC3 || T == IVEC3 || T == BVEC3) {
            return 3;
        } else {
            assert((T == VEC4 || T == IVEC4 || T == BVEC4));
            return 4;
        }
    }
}

type_code baseType(type_code T) {
    if (!isVector(T))
        return T;
    else {
        if (T == VEC2 || T == VEC3 || T == VEC4) {
            return FLOAT;
        } else if (T == IVEC2 || T == IVEC3 || T == IVEC4) {
            return INT;
        } else {
            assert((T == BVEC2 || T == BVEC3 || T == BVEC4));
            return BOOL;
        }
    }
}
/*---------------------------------------
Assumption:
1. For vv: &&, ||, == and != return scaler bool
2. For sv or vs: v and s must be in the same type (ex. ivec2 * int: correct. vec2 * int: incorrect)
3. For vv: * returns vector of the same type of operands
----------------------------------------*/
type_code deduceType(type_code a, type_code b, int op) {
    type_code ret;
    if (baseType(a) != baseType(b)) {
        std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match type";
        SEMANTIC_ERROR(msg.c_str());
        ret = ERROR;
    }
    if (op == 0 || op == 1) {
        // "&&" and "||"
        if (typeDimension(a) != typeDimension(b)) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match dimension";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = a;
    } else if (op == 2 || op == 3) {// "==" and "!="
        if (typeDimension(a) != typeDimension(b)) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match dimension";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = a;
    } else if (op == 4 || op == 5 || op == 6 || op ==7) {// "<", "<=", ">" and ">="
        if (isVector(a) || isVector(b)) {
            std::string msg = "ERROR: operand  to " 
                        + std::string(operator_name[op])
                        + " must be scalars";
            SEMANTIC_ERROR(msg.c_str());
        }
        if (a != b) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match type";
            SEMANTIC_ERROR(msg.c_str());
        }
        if (ret != ERROR) ret = a;
    } else if (op == 8 || op == 9){
        // "+" and "-"
        // the dimension must be the same
        if (typeDimension(a) != typeDimension(b)) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match dimension";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        // type must be arithmetic
        if (baseType(a) == BOOL || baseType(b) == BOOL) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " must be arithmetic type";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = a;
    } else if (op == 10){
        // "*"
        // must be the arithmetic
        if (baseType(a) == BOOL || baseType(b) == BOOL) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " must be arithmetic type";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) {
            if (!isVector(a) && !isVector(b)) {
                // two scalars, result is still scalar
                ret = a;
            } else {
                // one is vector, result is vector
                ret = isVector(a) ? a : b;
            }
        }
    } else if (op == 11 || op == 12){
        // "/" and "^"
        if (isVector(a) || isVector(b)) {
            std::string msg = "ERROR: operands to " 
                        + std::string(operator_name[op])
                        + " must be scalars";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = a;
    }
  return ret;
}

// get the type of the expression rooted at n
type_code getType(node *n) {
  assert(n != NULL);
  if (!n) return ERROR;
  node_kind kind = n->kind;
  type_code ret = ERROR;
  switch(kind) {
    case VAR_NODE:
      {
        ret = n->var_node.result_type;
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        ret = n->binary_expr.result_type;
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        // TODO: the resulting type must be of
        // the type of the expression
        ret = n->unary_expr.result_type;
        break;
      }
    case LITERAL_NODE:
      {
        ret = (type_code)n->literal.type;
        break;
      }
    case FUNCTION_NODE:
      {
        ret = n->function_node.result_type;
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        return n->constructor_node.result_type;
        break;
      }
    case TYPE_NODE:
        {
            return (type_code) n->type_node.code;
            break;
        }
    default: assert(false);
  }
  return ret;
}

int getNumArgs(node* n) {
    assert(n->kind == ARGUMENTS_NODE);
    int args = 0;
    if (n->arguments_node.right)
        args++;
    if (n->arguments_node.left) {
        // has left node
        if (n->arguments_node.left->kind == ARGUMENTS_NODE) {
            args+= getNumArgs(n->arguments_node.left);
        } else {
            args++;
        }
    }
    return args;
}

int getNumOfChildren(node *n) {
    if (!n) return 0;
    node_kind kind = n->kind;
    switch(kind){
        case ARGUMENTS_NODE: {
            if (n->arguments_node.right) {
                return 1 + getNumOfChildren(n->arguments_node.left);
            } else {
                return 0;
            }
            break;
        }
        default: {
            // FIXME:
            if (n) return 1;
            else return 0;
            break;
        }
    }
}

void typeCheck(node * n) {
  // FIXME
  // if (!n) return ERROR;
  if (!n) return;
  node_kind kind = n->kind;
  // type_code ret;
  switch(kind) {
    case SCOPE_NODE:
        {
            // entering a scope, create symbol table
            n->scope.symbol_table = new SYBL_T;
            // then push this table to the stack, as current scope
            symbol_stack.push_back(n->scope.symbol_table);
            // check declaration and statements
            if (n->scope.declarations) {
                typeCheck(n->scope.declarations);
            }
            if (n->scope.statements) {
                typeCheck(n->scope.statements);
            }
            // leaving current scope, pop it from stack
            symbol_stack.pop_back();
            break;
        }
    case DECLARATIONS_NODE:
        {
            if (n->declarations_node.left)
                typeCheck(n->declarations_node.left);
            if (n->declarations_node.right)
                typeCheck(n->declarations_node.right);
            break;
        }
    case DECLARATION_NODE:
        {
            // check if redefining variables
            auto current = symbol_stack.back();
            auto ret = current->insert(std::pair<std::string, struct symbol_attr>(
                            string(n->declaration_node.ident),
                            {.type = (type_code)n->declaration_node.kids[0]->type_node.code,
                                .predef = n->declaration_node.type == 2? CONST_VAR : (predef_attr) 0}));
            // insert failure means redefinition
            if (!ret.second) {
                SEMANTIC_ERROR("ERROR: redefining identifier");
            }
            // check intializer (expression) type, if exits
            if (n->declaration_node.type == 1 || n->declaration_node.type == 2){
                typeCheck(n->declaration_node.kids[1]);
                type_code init_type = getType(n->declaration_node.kids[1]);
                type_code decl_type = getType(n->declaration_node.kids[0]);
                if (init_type != decl_type) {
                    SEMANTIC_ERROR("ERROR: intializer does not match type of declaration");
                }
            }
            // TODO: insert to symbol table
            // if this decl is CONST qualified, check that
            // expression must be literal or UNIFORM variable
            if (n->declaration_node.type == 2) {
                if (n->declaration_node.kids[1]->kind != LITERAL_NODE
                    && n->declaration_node.kids[1]->kind != VAR_NODE) {
                    SEMANTIC_ERROR("ERROR: CONST variable not initialized with constant expression");
                }
                else if (n->declaration_node.kids[1]->kind == VAR_NODE) {
                    std::string id = n->declaration_node.kids[1]->var_node.ident;
                    if (id != "gl_Light_Half" && id != "gl_Light_Ambient" && id != "gl_Material_Shininess") {
                        SEMANTIC_ERROR("ERROR: CONST variable not initialized with constant expression");
                    }
                }
            }
            break;
        }
    case VAR_NODE:
      {
        /*VAR_NODE is a leaf*/
        // Check if variable is already declared
        auto entry = searchSymbolTable(n->var_node.ident);
        type_code ret = entry.second.type;
        if (ret == ERROR) {
            SEMANTIC_ERROR("ERROR: symbol not found");
        }
        n->var_node.attr = entry.second.predef;
        // If this variable is a vector indexing, check out of bound access
        if (n->var_node.type == 1) {

          if (ret == IVEC2 || ret == BVEC2 || ret == VEC2) {
            if (n->var_node.index > 1) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
            }
          } else if (ret == IVEC3 || ret == BVEC3 || ret == VEC3) {
            if (n->var_node.index > 2) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
            }
          } else if (ret == IVEC4 || ret == BVEC4 || ret == VEC4) {
            if (n->var_node.index > 3) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
            }
          }
          // report the basetype of the vector
          n->var_node.result_type = baseType(ret);
        } else {
            // update this var_node's data type
            n->var_node.result_type = ret;
        }
        break;
      }
    case ASSIGNMENT_NODE:
      {
        // check lhs of assignment (variable)
        typeCheck(n->assignment_node.left);
        // check rhs of assignment (expression)
        typeCheck(n->assignment_node.right);
        type_code lhs = getType(n->assignment_node.left);
        type_code rhs = getType(n->assignment_node.right);
        // check read-only variables and CONST variable
        predef_attr attr = n->assignment_node.left->var_node.attr;
        if (attr == ATTRIBUTE || attr == UNIFORM || attr == CONST_VAR) {
            SEMANTIC_ERROR("ERROR: cannot assign to read-only/constant variables");
        }
        if (lhs != rhs) {
            // lhs and rhs not of same type
            SEMANTIC_ERROR("ERROR: in assignment, lhs and rhs not of the same type");
        }
        // assignment node does not have a result type
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        type_code lhs, rhs;
        // descent to left
        if (n->binary_expr.left) {
            if (n->binary_expr.left->kind == BINARY_EXPRESSION_NODE) {
                // build left binary expr node
                typeCheck(n->binary_expr.left);
                lhs = getType(n->binary_expr.left);
            } else {
                typeCheck(n->binary_expr.left);
                lhs = getType(n->binary_expr.left);
            }
        }
        // get rhs
        if (n->binary_expr.right) {
            typeCheck(n->binary_expr.right);
            rhs = getType(n->binary_expr.right);
        }
        // the result type of the final expression depends on the
        // the sub expression and the type of operator
        n->binary_expr.result_type = deduceType(lhs, rhs, n->binary_expr.op);
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        // the type of the expression
        n->unary_expr.result_type = getType(n->unary_expr.expr);
        break;
      }
    case LITERAL_NODE:
      {
        // ret = (type_code)n->literal.type;
        break;
      }
    case FUNCTION_NODE:
      {
        // check args
        unsigned int num_args = 0;
        if (n->function_node.args) {
            if (n->function_node.args->kind == ARGUMENTS_NODE) {
                typeCheck(n->function_node.args);
                num_args = n->function_node.args->arguments_node.args_type.size();
            } else {
                typeCheck(n->function_node.args);
                num_args = 1;
            }
        }
        // depending on the function, the return type can be different
        if (n->function_node.type == 0) {
          // DP3
          if (num_args != 2){
            SEMANTIC_ERROR("ERROR: invalid number of arguments to DP3()");
            n->function_node.result_type = ERROR;
            return;
          }
          // check args type
          type_code first_arg = n->function_node.args->arguments_node.args_type[0];
          type_code second_arg = n->function_node.args->arguments_node.args_type[1];
          if ( first_arg == VEC4 && second_arg == VEC4) {
            n->function_node.result_type = FLOAT;
          } else if (first_arg == VEC3 && second_arg == VEC3) {
            n->function_node.result_type = FLOAT;
          } else if (first_arg == IVEC4 && second_arg == IVEC4) {
            n->function_node.result_type = INT;
          } else if (first_arg == IVEC3 && second_arg == IVEC3) {
            n->function_node.result_type = INT;
          } else {
            SEMANTIC_ERROR("ERROR: invalid arguments type to DP3");
            n->function_node.result_type = ERROR;
          }
        } else if(n->function_node.type == 1) {
            // vec4 lit(vec4)
            if (num_args != 1){
                SEMANTIC_ERROR("ERROR: invalid number of arguments to lit()");
                n->function_node.result_type = ERROR;
                return;
            }
            if (getType(n->function_node.args) == VEC4) {
                n->function_node.result_type = VEC4;
            } else {
                SEMANTIC_ERROR("ERROR: invalid arguments to lit()");
                n->function_node.result_type = ERROR;
            }
        } else {
            // rsq
            assert(n->function_node.type == 2);
            if (num_args != 1) {
                SEMANTIC_ERROR("ERROR: invalid number of arguments to rsq()");
                n->function_node.result_type = ERROR;
                return;
            }
            if (getType(n->function_node.args) == FLOAT 
                || getType(n->function_node.args) == INT) {
                n->function_node.result_type = FLOAT;
            } else {
                SEMANTIC_ERROR("ERROR: invalid arguments types to rsq()");
                n->function_node.result_type = ERROR;
            }

        }
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        // constructor type
        typeCheck(n->constructor_node.type);
        type_code ctor_type = getType(n->constructor_node.type);
        unsigned int num_args = 0;
        // check args if exists
        if (n->constructor_node.arguments){
            if (n->constructor_node.arguments->kind == ARGUMENTS_NODE) {
                typeCheck(n->constructor_node.arguments);
                num_args = n->constructor_node.arguments->arguments_node.args_type.size();
            } else {
                // only single expression
                typeCheck(n->constructor_node.arguments);
                num_args = 1;
            }
        }
        if (num_args != (unsigned int)typeDimension(ctor_type)){
            SEMANTIC_ERROR("ERROR: invalid number of arguments to constructor");            
            n->constructor_node.result_type = ERROR;
            return;
        }
        // check if args is the same type
        if (num_args == 1) {
            if (getType(n->constructor_node.arguments) != ctor_type){
                SEMANTIC_ERROR("ERROR: invalid arguments type to constructor");
                n->constructor_node.result_type = ERROR;
            }
        } else {
            if (n->constructor_node.arguments) {
                for (type_code T : n->constructor_node.arguments->arguments_node.args_type) {
                    if (T != baseType(ctor_type)) {
                        SEMANTIC_ERROR("ERROR: invalid arguments type to constructor");
                        n->constructor_node.result_type = ERROR;
                    }
                }
            }
        }
        n->constructor_node.result_type = ctor_type;
        break;
      }
    case STATEMENTS_NODE:
        {
            if (n->statements_node.left)
                typeCheck(n->statements_node.left);
            if (n->statements_node.right)
                typeCheck(n->statements_node.right);
            break;
        }
    case IF_STATEMENT_NODE:
        {
            // check condition
            typeCheck(n->if_stmt_node.kids[0]);
            type_code cond_type = getType(n->if_stmt_node.kids[0]);
            if (cond_type != BOOL){
                SEMANTIC_ERROR("ERROR: invalid type to condition");
            }
            // check then_statement
            typeCheck(n->if_stmt_node.kids[1]);
            // check else_statement if exists
            if (n->if_stmt_node.withElse) {
                assert (n->if_stmt_node.kids[2] != NULL);
                typeCheck(n->if_stmt_node.kids[2]);
            }
            break;
        }
    case ARGUMENTS_NODE:
        {
            // get left arguments
            assert(n!=NULL);
            if (n->arguments_node.left) {
                if (n->arguments_node.left->kind == ARGUMENTS_NODE) {
                    // build left arguments node
                    typeCheck(n->arguments_node.left);
                    n->arguments_node.args_type = n->arguments_node.left->arguments_node.args_type;
                } else {
                    // left argument is an expression
                    typeCheck(n->arguments_node.left);
                    n->arguments_node.args_type.push_back(getType(n->arguments_node.left));
                }
            }
            // get current arguments
            if (n->arguments_node.right) {
                typeCheck(n->arguments_node.right);
                n->arguments_node.args_type.push_back(getType(n->arguments_node.right));
            }
            break;
        }
    case TYPE_NODE:
        {
            break;
        }
    default: assert(false);
  }
}

int semantic_check() {
    // 1. build symbol table
    // buildSymbolTable(ast);
    // 2. type check, and update resulting type in nodes
    typeCheck(ast);
  return 0; // failed checks
}

