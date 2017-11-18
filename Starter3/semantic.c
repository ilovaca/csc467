
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
// type_code deduceType(type_code a, type_code b, int op) {
//   // 4 cases, a, b both can be either scalar or vector
//   type_code ret = ERROR;
//   if (isVector(a) && isVector(b)) {

//   } else if (!isVector(a) && isVector(b)) {

//   } else if (isVector(a) && !isVector(b)) {

//   } else {
//     // !isVector(a) && !isVector(b)
//   }
//   return ret;
// }

// get the type of the expression rooted at n
type_code getType(node *n) {
  assert(n != NULL);
  if (!n) return ERROR;
  node_kind kind = n->kind;
  type_code ret;
  switch(kind) {
    case VAR_NODE:
      {
        // TODO: consult symbol table, from inner scope to outer scope
        ret = searchSymbolTable(n->var_node.ident);
        // FIXME: is ivec3[1] of type INT or IVEC3?
        if (ret == ERROR) {
          cout << "ERROR: symbol not found" << endl;
        }
        if (n->var_node.type == 1){
          // this is a vector indexing
          if (ret == IVEC2 || ret == IVEC3 || ret == IVEC4) {
            ret = INT;
          } else if (ret == BVEC2 || ret == BVEC3 || ret == BVEC4) {
            ret = BOOL;
          } else if (ret == VEC2 || ret ==  VEC3 || ret == VEC4) {
            ret = FLOAT;
          }
        }

        break;
      }
    case ASSIGNMENT_NODE:
      {
        // TODO: consult symbol table
        ret = searchSymbolTable(n->assignment_node.left->var_node.ident);
        if (ret == ERROR)
          cout << "ERROR: symbol not found" << endl;
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        // TODO: deduce type
        // the "resulting" type of this expression should 
        // be of the base type, but not necessarily the same type
        // get the type of two sub expressions
        // type_code lhs, rhs;
        // if (n->binary_expr.left) {
        //   lhs = getType(n->binary_expr.left);
        // }
        // if (n->binary_expr.right) {
        //   rhs = getType(n->binary_expr.right);
        // }
        // if (lhs == ERROR || rhs == ERROR) {
        //   cout << "FATAL ERROR" << endl;
        // }
        // the result type of the final expression depends on the
        // the sub expression and the type of operator
        // ret = deduceType(lhs, rhs, n->binary_expr.op);
        ret = n->binary_expr.result_type;
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        // TODO: the resulting type must be of
        // the type of the expression
        ret = getType(n->unary_expr.expr);
        break;
      }
    case LITERAL_NODE:
      {
        ret = (type_code)n->literal.type;
        break;
      }
    case FUNCTION_NODE:
      {
        // depending on the function, the return type can be different
        if (ast->function_node.type == 0) {
          // DP3
          // if param is vec3/4, then return FLOAT
          if (getType(n->function_node.args) == VEC3
              || getType(n->function_node.args) == VEC4) {
            return FLOAT;
          } else if ( getType(n->function_node.args) == VEC3
                || getType(n->function_node.args) == VEC4) {
            return INT;
          }
        } else if(ast->function_node.type == 1) {
          // lit
          return VEC4;
        } else {
          // rsq
          assert(ast->function_node.type == 2);
          return FLOAT;
        }
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        return (type_code)n->constructor_node.type->type_node.code;
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

type_code typeCheck(node * n) {
  assert(n != NULL);
  // FIXME
  if (!n) return ERROR;
  node_kind kind = n->kind;
  type_code ret;
  switch(kind) {
    case SCOPE_NODE:
        {
            // check declaration and statements
            if (n->scope.declarations) {
                typeCheck(n->scope.declarations);
            }
            if (n->scope.statements) {
                typeCheck(n->scope.statements);
            }
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
            // check intializer (expression) type
            typeCheck(n->declaration_node.kids[1]);
            type_code init_type = getType(n->declaration_node.kids[1]);
            type_code decl_type = getType(n->declaration_node.kids[0]);
            if (init_type != decl_type) {
                SEMANTIC_ERROR("ERROR: intializer does not match type of declaration");
            }
            // TODO: insert to symbol table
            // if this decl is CONST qualified, check
            // expression must be literal or UNIFORM variable
            if (n->declaration_node.type == 2) {
                if (n->declaration_node.kids[1]->kind != LITERAL_NODE
                    && n->declaration_node.kids[1]->kind != VAR_NODE) {
                    SEMANTIC_ERROR("CONST variable not initialized with constant expression");
                }
                else if (n->declaration_node.kids[1]->kind == VAR_NODE) {
                    std::string id = n->declaration_node.kids[1]->var_node.ident;
                    if (id != "gl_Light_Half" && id != "gl_Light_Ambient" && id != "gl_Material_Shininess") {
                        SEMANTIC_ERROR("CONST variable not initialized with constant expression");
                    }
                }
            }
            break;
        }
    case VAR_NODE:
      {
        /*VAR_NODE is a leaf*/
        // Check if variable is already declared
        ret = searchSymbolTable(n->var_node.ident);
        // FIXME: is ivec3[1] of type INT or IVEC3?
        if (ret == ERROR) {
            cout << "ERROR: symbol not found" << endl;
            SEMANTIC_ERROR("ERROR: symbol not found");
        }
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
        }
        // update this var_node's data type
        n->var_node.result_type = ret;
        break;
      }
    case ASSIGNMENT_NODE:
      {
        // check lhs of assignment
        type_code lhs = searchSymbolTable(n->assignment_node.left->var_node.ident);
        if (lhs == ERROR) {
            cout << "ERROR: symbol not found" << endl;
            SEMANTIC_ERROR("ERROR: symbol not found");
        }
        // check rhs of assignment (expression)
        type_code rhs = getType(n->assignment_node.right);
        if (rhs == ERROR) {
            //pass
        }
        if (lhs != rhs) {
            // lhs and rhs not of same type
            SEMANTIC_ERROR("ASSIGNMENT_NODE, lhs and rhs not of the same type");
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
                lhs = getType(n->binary_expr.left);
            }
        }
        // get rhs
        if (n->binary_expr.right) {
            rhs = getType(n->binary_expr.right);
        }
        // the result type of the final expression depends on the
        // the sub expression and the type of operator
        n->binary_expr.result_type = deduceType(lhs, rhs, n->binary_expr.op);
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        // TODO: the resulting type must be of
        // the type of the expression
        ret = getType(n->unary_expr.expr);
        break;
      }
    case LITERAL_NODE:
      {
        ret = (type_code)n->literal.type;
        break;
      }
    case FUNCTION_NODE:
      {
        // depending on the function, the return type can be different
        if (n->function_node.type == 0) {
          // DP3
          if (getNumArgs(n->function_node.args) != 2){
            SEMANTIC_ERROR("ERROR: invalid number of arguments to DP3()");
          }
          // check args type
          type_code first_arg = getType(n->function_node.args->arguments_node.right);
          type_code second_arg = getType(n->function_node.args->arguments_node.left->arguments_node.right);
          if ( first_arg != VEC4 || second_arg != VEC4) {
            SEMANTIC_ERROR("ERROR: invalid arguments type to DP3");
          } else if ( first_arg != VEC3 || second_arg != VEC3) {
            SEMANTIC_ERROR("ERROR: invalid arguments type to DP3");
          } else if (first_arg != IVEC4 || second_arg != IVEC4) {
            SEMANTIC_ERROR("ERROR: invalid arguments type to DP3");
          } else if (first_arg != IVEC3 || second_arg != IVEC3) {
            SEMANTIC_ERROR("ERROR: invalid arguments type to DP3");
          }
        } else if(n->function_node.type == 1) {
            // vec4 lit(vec4)
            if (getNumArgs(n->function_node.args) != 1){
                SEMANTIC_ERROR("ERROR: invalid number of arguments to lit()");
            }
            if (getType(n->function_node.args) != VEC4) {
                SEMANTIC_ERROR("ERROR: invalid arguments to lit()");
            }
        } else {
          // rsq
          assert(n->function_node.type == 2);
          if (getNumArgs(n->function_node.args) != 1) {
            SEMANTIC_ERROR("ERROR: invalid number of arguments to rsq()");
          }
          if (getType(n->function_node.args) != FLOAT 
                || getType(n->function_node.args) != INT) {
                SEMANTIC_ERROR("ERROR: invalid arguments types to rsq()");
            }
        }
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        // check args
        typeCheck(n->constructor_node.arguments);
        int num_args = getNumArgs(n->constructor_node.arguments);
        type_code ctor_type = getType(n);
        // type_code arg_type = getType(n->constructor_node.arguments);
        // check arguments number
        if (num_args != typeDimension(ctor_type)) {
            SEMANTIC_ERROR("ERROR: invalid number of arguments to constructor");
        }
        auto& args_vec = n->constructor_node.arguments->arguments_node.args_type;
        if (args_vec.size() != (unsigned int)typeDimension(ctor_type)){
            SEMANTIC_ERROR("ERROR: invalid number of arguments to constructor");            
        }
        // check arguments type
        // if (ctor_type != baseType(arg_type)){
        // }
        for (type_code T : args_vec) {
            if (T != baseType(ctor_type)) {
                SEMANTIC_ERROR("ERROR: invalid arguments type to constructor");
            }
        }
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
            type_code cond_type = getType(n->if_stmt_node.kids[0]);
            if (cond_type != BOOL)
                SEMANTIC_ERROR("ERROR: invalid type to condition");
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
            if (n->arguments_node.left) {
                if (n->arguments_node.left->kind == ARGUMENTS_NODE) {
                    // build left arguments node
                    typeCheck(n->arguments_node.left);
                    n->arguments_node.args_type = n->arguments_node.left->arguments_node.args_type;
                } else {
                    // left argument is an expression
                    n->arguments_node.args_type.push_back(getType(n->arguments_node.left));
                }
            }
            // get current arguments
            if (n->arguments_node.right) {
                n->arguments_node.args_type.push_back(getType(n->arguments_node.right));
            }
            break;
        }
    default: assert(false);
  }
  return ret;
}

int semantic_check() {
    // 1. build symbol table
    buildSymbolTable(ast);
    // 2. type check, and update resulting type in nodes
    typeCheck(ast);
  return 0; // failed checks
}

