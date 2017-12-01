
#include "semantic.h"
#include "common.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include "parser.tab.h"
#include <set>
using namespace std;
/********** For Checking Write-only Variables *********/
bool insideIfElse = false;
bool insideAssignStatement = false;
node* cur_node = NULL;
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
        if (baseType(a) != BOOL || baseType(b) != BOOL) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " must be boolean types";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = BOOL;
    } else if (op == 2 || op == 3) {// "==" and "!="
        if (typeDimension(a) != typeDimension(b)) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match dimension";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = BOOL;
    } else if (op == 4 || op == 5 || op == 6 || op ==7) {// "<", "<=", ">" and ">="
        if (isVector(a) || isVector(b)) {
            std::string msg = "ERROR: operand  to " 
                        + std::string(operator_name[op])
                        + " must be scalars";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (a != b) {
            std::string msg = "ERROR: operand type to " 
                        + std::string(operator_name[op])
                        + " does not match type";
            SEMANTIC_ERROR(msg.c_str());
            ret = ERROR;
        }
        if (ret != ERROR) ret = BOOL;
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


void semantic_check(node * n) {
  if (!n) return;
  cur_node = n;
  node_kind kind = n->kind;
  switch(kind) {
    case SCOPE_NODE:
        {
            // entering a scope, create symbol table
            n->scope.symbol_table = new SYBL_T;
            // then push this table to the stack, as current scope
            symbol_stack.push_back(n->scope.symbol_table);
            // check declaration and statements
            if (n->scope.declarations) {
                semantic_check(n->scope.declarations);
            }
            if (n->scope.statements) {
                semantic_check(n->scope.statements);
            }
            // leaving current scope, pop it from stack
            symbol_stack.pop_back();
            break;
        }
    case DECLARATIONS_NODE:
        {
            if (n->declarations_node.left)
                semantic_check(n->declarations_node.left);
            if (n->declarations_node.right)
                semantic_check(n->declarations_node.right);
            break;
        }
    case DECLARATION_NODE:
        {
            // check for predefined variables
            if (searchPredefined(n->declaration_node.ident).second.type != ERROR) {
                SEMANTIC_ERROR("ERROR: identifier already predefined");
            }
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
                semantic_check(n->declaration_node.kids[1]);
                type_code init_type = getType(n->declaration_node.kids[1]);
                type_code decl_type = getType(n->declaration_node.kids[0]);
                if (init_type != decl_type) {
                    SEMANTIC_ERROR("ERROR: intializer does not match type of declaration");
                }
            }
            // if this decl is CONST qualified, check that
            // expression must be literal or UNIFORM variable
            // if (n->declaration_node.type == 2) {
            //     if (n->declaration_node.kids[1]->kind != LITERAL_NODE
            //         && n->declaration_node.kids[1]->kind != VAR_NODE) {
            //         SEMANTIC_ERROR("ERROR: CONST variable not initialized with constant expression");
            //     }
            //     else if (n->declaration_node.kids[1]->kind == VAR_NODE) {
            //         std::string id = n->declaration_node.kids[1]->var_node.ident;
            //         if (id != "gl_Light_Half" && id != "gl_Light_Ambient" && id != "gl_Material_Shininess") {
            //             SEMANTIC_ERROR("ERROR: CONST variable not initialized with constant expression");
            //         }
            //     }
            // }
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
            ret = ERROR;
        }
        // update attributes
        n->var_node.attr = entry.second.predef;
        // check for write-only, can only be inside assignments
        if (!insideAssignStatement && (n->var_node.attr == RESULT)) {
            SEMANTIC_ERROR("ERROR: reading a Write-only variable");
            ret = ERROR;
        }
        // If this variable is a vector indexing, check out of bound access
        if (n->var_node.type == 1) {

          if (ret == IVEC2 || ret == BVEC2 || ret == VEC2) {
            if (n->var_node.index > 1) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
                ret = ERROR;
            }
          } else if (ret == IVEC3 || ret == BVEC3 || ret == VEC3) {
            if (n->var_node.index > 2) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
                ret = ERROR;
            }
          } else if (ret == IVEC4 || ret == BVEC4 || ret == VEC4) {
            if (n->var_node.index > 3) {
                SEMANTIC_ERROR("ERROR: out of bound vector access");
                ret = ERROR;
            }
          }
          // report the basetype of the vector
          if (ret != ERROR) n->var_node.result_type = baseType(ret);
        } else {
            // update this var_node's data type
            if (ret != ERROR) n->var_node.result_type = ret;
        }
        break;
      }
    case ASSIGNMENT_NODE:
      {
        // check lhs of assignment (variable)
        insideAssignStatement = true;
        semantic_check(n->assignment_node.left);
        insideAssignStatement = false;
        // check rhs of assignment (expression)
        semantic_check(n->assignment_node.right);
        type_code lhs = getType(n->assignment_node.left);
        type_code rhs = getType(n->assignment_node.right);
        // check read-only variables and CONST variable
        predef_attr attr = n->assignment_node.left->var_node.attr;
        if (attr == ATTRIBUTE || attr == UNIFORM || attr == CONST_VAR) {
            SEMANTIC_ERROR("ERROR: cannot assign to read-only/constant variables");
        }
        // check for write only variables
        if (insideIfElse) {
            if (n->assignment_node.left->var_node.attr == RESULT){
                SEMANTIC_ERROR("ERROR: cannot assign to Result variables inside IF-ELSE statements");
            }
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
                semantic_check(n->binary_expr.left);
                lhs = getType(n->binary_expr.left);
            } else {
                semantic_check(n->binary_expr.left);
                lhs = getType(n->binary_expr.left);
            }
        }
        // get rhs
        if (n->binary_expr.right) {
            semantic_check(n->binary_expr.right);
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
        semantic_check(n->unary_expr.expr);
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
                semantic_check(n->function_node.args);
                num_args = n->function_node.args->arguments_node.args_type.size();
            } else {
                semantic_check(n->function_node.args);
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
        semantic_check(n->constructor_node.type);
        type_code ctor_type = getType(n->constructor_node.type);
        unsigned int num_args = 0;
        // check args if exists
        if (n->constructor_node.arguments){
            if (n->constructor_node.arguments->kind == ARGUMENTS_NODE) {
                semantic_check(n->constructor_node.arguments);
                num_args = n->constructor_node.arguments->arguments_node.args_type.size();
            } else {
                // only single expression
                semantic_check(n->constructor_node.arguments);
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
                semantic_check(n->statements_node.left);
            if (n->statements_node.right)
                semantic_check(n->statements_node.right);
            break;
        }
    case IF_STATEMENT_NODE:
        {
            // check condition
            semantic_check(n->if_stmt_node.kids[0]);
            type_code cond_type = getType(n->if_stmt_node.kids[0]);
            if (cond_type != BOOL){
                SEMANTIC_ERROR("ERROR: invalid type to condition");
            }
            // check then_statement
            insideIfElse = true;
            semantic_check(n->if_stmt_node.kids[1]);
            insideIfElse = false;
            // check else_statement if exists
            if (n->if_stmt_node.withElse) {
                assert (n->if_stmt_node.kids[2] != NULL);
                insideIfElse = true;
                semantic_check(n->if_stmt_node.kids[2]);
                insideIfElse = false;
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
                    semantic_check(n->arguments_node.left);
                    n->arguments_node.args_type = n->arguments_node.left->arguments_node.args_type;
                } else {
                    // left argument is an expression
                    semantic_check(n->arguments_node.left);
                    n->arguments_node.args_type.push_back(getType(n->arguments_node.left));
                }
            }
            // get current arguments
            if (n->arguments_node.right) {
                semantic_check(n->arguments_node.right);
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
string codegen(node * n, int reg_id );

// #define CODEGEN(x) {out << x << endl;}

std::ofstream out ("frag.txt", std::ios::out);
void codegen(){
    if (!out) {
        return;
    }
    out << "!!ARBfp1.0" << endl;
    codegen(ast, 0);
    out << "END"<< endl;
    out.close();
}
int reg_bound = -1;
std::set<int> reg_set;

std::string alloc_reg(int reg_id) {
    if (reg_set.find(reg_id) == reg_set.end()) {
        // reg_bound = reg_id;
        reg_set.insert(reg_id);
        return "TEMP tempVar" + to_string( reg_id);
    } else {
        return "";
    }
}

std::string alloc_const_reg(int reg_id) {
    if (reg_set.find(reg_id) == reg_set.end()) {
        reg_set.insert(reg_id);
        return "PARAM tempVar" + to_string( reg_id);
    } else {
        return "";
    }
}

std::string alloc_reg(std::string s) {
    return "TEMP " + s;
}


std::string getRegName(node * n) {
    if (!n) return "";
    node_kind kind = n->kind;
    std::string ret = "";
    switch(kind) {
        case VAR_NODE:{
            ret = n->var_node.reg_name;
            break;
        }
        case LITERAL_NODE:{
            ret = n->literal.reg_name;
            break;
        }
        case CONSTRUCTOR_NODE:{
            ret = n->constructor_node.reg_name;
            break;
        }
        case FUNCTION_NODE: {
            ret = n->function_node.reg_name;
            break;
        }
        case BINARY_EXPRESSION_NODE: {
            ret = n->binary_expr.reg_name;
            break;
        }
        case UNARY_EXPRESION_NODE: {
            ret = n->unary_expr.reg_name;
            break;
        }
        default: assert(false);
    }
    return ret;
}


const char index[4] = {'x', 'y', 'z', 'w'};


string if_cond_reg = "";
bool insideTHEN = false;
bool insideELSE = false;
string codegen(node * n, int reg_id = 0) {
  if (!n) return "";
  cur_node = n;
  // out << alloc_reg(reg_id) << endl;
  node_kind kind = n->kind;
  switch(kind) {
    case SCOPE_NODE:
        {
            // entering a scope, push current symbol table to stack
            symbol_stack.push_back(n->scope.symbol_table);
            // SCOPE node does not consume registers.
            // declaration and statements
            if (n->scope.declarations) {
                codegen(n->scope.declarations, reg_id );
            }
            if (n->scope.statements) {
                codegen(n->scope.statements, reg_id );
            }
            // leaving current scope, pop it from stack
            symbol_stack.pop_back();
            break;
        }
    case DECLARATIONS_NODE:
        {
            if (n->declarations_node.left)
                codegen(n->declarations_node.left, reg_id);
            if (n->declarations_node.right)
                codegen(n->declarations_node.right, reg_id);
            break;
        }
    case DECLARATION_NODE:
        {
            std::string var_name = n->declaration_node.ident;
            out << alloc_reg(var_name) << endl;
            // code gen for expression if it exists
            if (n->declaration_node.type == 1 || n->declaration_node.type == 2){
                codegen(n->declaration_node.kids[1], reg_id);
                // initialize the declaration 
                out << "MOV " << var_name << " , " << getRegName(n->declaration_node.kids[1]) << ";" << endl;
            }
            break;
        }
    case VAR_NODE:
      {
        std::string ident = n->var_node.ident;
        if (ident == "gl_FragColor") {
            ident = "result.color";
        } else if (ident == "gl_FragDepth") {
            ident = "result.depth";
        } else if (ident == "gl_FragCoord") {
            ident = "fragment.position";
        } else if (ident == "gl_TexCoord") {
            ident = "fragment.texcoord";
        } else if (ident == "gl_Color") {
            ident = "fragment.color";
        } else if (ident == "gl_Secondary") {
            ident = "fragment.color.secondary";
        } else if (ident == "gl_FogFragCoord") {
            ident = "fragment.fogcoord";
        } else if (ident == "gl_Light_Half") {
            ident = "state.light[0].half";
        } else if (ident == "gl_Light_Ambient"){
            ident = "state.lightmodel.ambient";
        } else if (ident == "gl_Material_Shininess") {
            ident = "state.material.shininess";
        } else if (ident == "env1") {
            ident = "program.env[1]";
        } else if (ident == "env2") {
            ident = "program.env[2]";
        } else if (ident == "env3") {
            ident = "program.env[3]";
        } else {

        }
        if (n->var_node.type == 0) {
            // scalar
            n->var_node.reg_name = ident;
        } else {
            n->var_node.reg_name = ident + string(".") + index[n->var_node.index];
        }

        // return n->var_node.reg_name;
        break;
      }
    case ASSIGNMENT_NODE:
      {
        // check lhs of assignment (variable)
        insideAssignStatement = true;
        codegen(n->assignment_node.left, reg_id);
        // auto var = n->assignment_node.left->var_node.ident;
        auto var = getRegName( n->assignment_node.left);
        insideAssignStatement = false;
        // check rhs of assignment (expression)
        codegen(n->assignment_node.right, reg_id );
        // check the type of variable
        if (!insideIfElse) {
            out << "MOV " << var << ", " << getRegName(n->assignment_node.right) << ";"<< endl;
        } else {
            // inside if-else, do CMP
            if (insideTHEN) {
                // THEN statement
                out << "CMP " << var << ", " << if_cond_reg << ", " << var << ", " << getRegName(n->assignment_node.right) << ";"<< endl;
            } else {
                assert(insideELSE);
                out << "CMP " << var << ", " << if_cond_reg << ", " << getRegName(n->assignment_node.right)  << " , " << var<< ";" << endl;
            }
        }
        break;
      }
    case BINARY_EXPRESSION_NODE:
      {
        codegen(n->binary_expr.left, reg_id + 1);
        codegen(n->binary_expr.right, reg_id + 2);
        out << alloc_reg(reg_id) << endl;
        // depends on the operation type...
        switch(n->binary_expr.op){
            case 0: {
                // "&&"
                // FIXME
                out << "ADD " << "tempVar" << reg_id << ", tempVar" << reg_id + 1
                    << ", tempVar" << reg_id + 2 << endl;
                break;
            }
            case 8: {
                // +, -, *
                // out << "ADD " << "tempVar" << reg_id << ", tempVar" << reg_id + 1
                //     << ", tempVar" << reg_id + 2 << endl;
                out << "ADD " << "tempVar" << reg_id << ", " << getRegName(n->binary_expr.left)
                    << " , " << getRegName(n->binary_expr.right)<< ";" << endl;
                break;
            }
            case 9: {
                // out << "SUB " << "tempVar" << reg_id << ", tempVar" << reg_id + 1
                //     << ", tempVar" << reg_id + 2 << endl;
                out << "SUB " << "tempVar" << reg_id << ", " << getRegName(n->binary_expr.left)
                    << " , " << getRegName(n->binary_expr.right)<< ";" << endl;
                break;
            }
            case 10: {
                // out << "MUL " << "tempVar" << reg_id << ", tempVar" << reg_id + 1
                //     << ", tempVar" << reg_id + 2 << endl;
                out << "MUL " << "tempVar" << reg_id << ", " << getRegName(n->binary_expr.left)
                    << " , " << getRegName(n->binary_expr.right)<< ";" << endl;
                break;
            }
            case 11: {
                // find reciprocal of second operand
                out << "RCP " << "tempVar" << reg_id + 2 << ", tempVar" << reg_id + 2  << ".x"<< ";"<< endl;
                out << "MUL " << "tempVar" << reg_id << ", tempVar" << reg_id + 1
                    << ", tempVar" << reg_id + 2<< ";" << endl;
                break;
            }
            case 12: {
                // "^"
                // FIXME: EXP is base-two
                // out << "EXP " << "tempVar" << reg_id << " ,"
                break;
            }
            default: break;
        }
        n->binary_expr.reg_name = "tempVar" + to_string(reg_id);
        break;
      }
    case UNARY_EXPRESION_NODE:
      {
        codegen(n->unary_expr.expr, reg_id + 1);
        //
        out << alloc_reg(reg_id) << endl;
        int op = n->unary_expr.op;
        if (op == 0) {
            // "-" expr = 0 SUB expr
            out << "MOV " << "tempVar" << reg_id << ", {0,0,0,0}"<< ";" << endl;
            out << "SUB " << "tempVar" << reg_id << ", tempVar" << reg_id << ", tempVar" << reg_id + 1 << ";"<< endl;
        } else {
            // "!"
        }
        n->unary_expr.reg_name = "tempVar" + to_string(reg_id);
        break;
      }
    case LITERAL_NODE:
      {
        // literal is a scalar
        // out << "PARAM " << "tempVar" << reg_id  << " = ";
        out << alloc_reg(reg_id) << ";"<< endl;
        out << "MOV " << "tempVar" << reg_id << ", ";
        if (n->literal.type == INT) {
            // INT type is signified by 3 zeros 
            out << "{ " << n->literal.ival << ", 0, 0, 0}" << ";"<< endl;
        } else if (n->literal.type == FLOAT){
            out << "{ " << n->literal.fval << ", 1, 1, 1}" << ";"<< endl;
        } else {
            // BOOL type is treated the same as INT, where 1 is true,
            // -1 is false;
            int bval = n->literal.bval? 1.0 : 0.0;
            out << "{ " << bval << ", 0, 0, 0}" << ";"<< endl;
        }
        n->literal.reg_name = "tempVar" + to_string(reg_id);
        break;
      }
    case FUNCTION_NODE:
      {
        codegen(n->function_node.args, reg_id + 1);
        out << alloc_reg(reg_id) << endl;
        // depending on the function, the return type can be different
        if (n->function_node.type == 0) {
            // DP3 has two args
            // FIXME: need to conserve the register  from reg_id+1 to the number of arguments
            auto regs = n->function_node.args->arguments_node.reg_name;
            assert(regs.size() == 2);
            out << "DP3 " << " tempVar" << reg_id << ", " << regs[0] 
                                                  << ", " << regs[1] << ";"<< endl;
        } else if(n->function_node.type == 1) {
            // vec4 lit(vec4)
            // out << "LIT " << " tempVar" << reg_id << ", tempVar" << reg_id + 1 << endl;
            out << "LIT " << " tempVar" << reg_id << "," << getRegName(n->function_node.args) << ";"<< endl;
        } else {
            assert(n->function_node.type == 2);
            // rsq(float/int)
            // codegen(n->function_node.args, reg_id + 1);
            // out << "RSQ " << " tempVar" << reg_id << ", tempVar" << reg_id + 1 << endl;
            out << "RSQ " << " tempVar" << reg_id << "," << getRegName(n->function_node.args) << ";"<< endl;
        }
        n->function_node.reg_name = "tempVar" + to_string(reg_id);
        break;
      }
    case CONSTRUCTOR_NODE:
      {
        // constructor type
        // codegen(n->constructor_node.type);
        // check args if exists
        if (n->constructor_node.arguments->kind != ARGUMENTS_NODE) {
            // only one argument
            codegen(n->constructor_node.arguments, reg_id + 1);
            out << alloc_reg(reg_id) << endl; 
            std::string reg_name;
            reg_name = getRegName(n->constructor_node.arguments);
            out << "MOV tempVar" << reg_id << ".x" << ", " << reg_name << ";"<< endl;
        } else {
            // multiple args
            auto num_args = getNumArgs(n->constructor_node.arguments);
            // destination
            out << alloc_reg(reg_id) << endl;
            auto arg = n->constructor_node.arguments;
            for (int i = num_args - 1; i != 0; i--) {
                // reg_id++;
                codegen(arg->arguments_node.right, reg_id + 1);
        
                out << "MOV tempVar" << reg_id <<  "." << index[i] << ", tempVar" << reg_id + 1 << ";"<< endl;
                if (arg->arguments_node.left->kind == ARGUMENTS_NODE) {
                    arg = arg->arguments_node.left;
                } else {
                    // left node is an expression;
                    codegen(arg->arguments_node.left, reg_id + 1);
                    out << "MOV tempVar" << reg_id <<  "." << index[i - 1] << ", tempVar" << reg_id + 1<< ";" << endl;
                    break;
                }
            }
        }
        n->constructor_node.reg_name = "tempVar" + to_string(reg_id);
        break;
      }
    case STATEMENTS_NODE:
        {
            if (n->statements_node.left)
                codegen(n->statements_node.left, reg_id);
            if (n->statements_node.right)
                codegen(n->statements_node.right, reg_id);
            break;
        }
    case IF_STATEMENT_NODE:
        {
            // generate condition
            codegen(n->if_stmt_node.kids[0], reg_id);
            // the condition register
            // out << alloc_reg(reg_id) << endl;
            if_cond_reg = getRegName(n->if_stmt_node.kids[0]);
            // then_statement
            insideIfElse = true;
            insideTHEN = true;
            codegen(n->if_stmt_node.kids[1], reg_id + 1);
            insideIfElse = false;
            insideTHEN = false;
            // gen else_statement if exists
            if (n->if_stmt_node.withElse) {
                assert (n->if_stmt_node.kids[2] != NULL);
                insideIfElse = true;
                insideELSE = true;
                codegen(n->if_stmt_node.kids[2], reg_id + 2);
                insideELSE = false;
                insideIfElse = false;
            }
            break;
        }
    case ARGUMENTS_NODE:
        {
            codegen(n->arguments_node.left, reg_id);
            codegen(n->arguments_node.right, reg_id + 1);
            // out << alloc_reg(reg_id) << endl;
            if (n->arguments_node.left) {
                if (n->arguments_node.left->kind == ARGUMENTS_NODE) {
                    // n->arguments_node.reg_name = n->arguments_node.left->arguments_node.reg_name;
                    // if left node is a ARGUMENT_NODE, then append its list of registers over
                    n->arguments_node.reg_name.insert(n->arguments_node.reg_name.end(),
                                n->arguments_node.left->arguments_node.reg_name.begin(),
                                n->arguments_node.left->arguments_node.reg_name.end());
                } else {
                    // left argument is an expression, just append it
                    n->arguments_node.reg_name.push_back(getRegName(n->arguments_node.left));
                }
            }
            // append right registers
            if (n->arguments_node.right) {
                n->arguments_node.reg_name.push_back(getRegName(n->arguments_node.right));
            }
            // get number
            break;
        }
    case TYPE_NODE:
        {
            break;
        }
    default: assert(false);
    }
    return "";
}


std::vector<string> getArgRegs(node * n) {
    std::vector<string> ret;
    if (n == NULL) return ret;

}