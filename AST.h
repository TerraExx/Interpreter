/*
 * AST.h
 *
 *  Created on: Dec 2, 2017
 *      Author: Josip
 */

#ifndef INTERPRETER_AST_H_
#define INTERPRETER_AST_H_

/********* AST Nodes *********/

typedef struct
{
	s_lexer_token	token;
}s_ast_factor;

typedef struct
{
	s_lexer_token	token;
}s_ast_type_spec;

typedef struct s_ast_factor_link
{
    s_ast_factor*   factor;
    struct s_ast_factor_link*   next_factor_link;
}s_ast_factor_link;

typedef struct
{
	s_ast_type_spec*	type_spec;
	s_ast_factor_link*	factor_head_link;
}s_ast_variable_declaration;

typedef struct s_ast_variable_declaration_link
{
	s_ast_variable_declaration* variable_declaration;
	struct s_ast_variable_declaration_link* next_variable_declaration_link;
}s_ast_variable_declaration_link;

typedef struct s_ast_variable_declaration
{
    s_ast_variable_declaration_link*    variable_declaration_head_link;
}s_ast_declarations;

typedef struct
{

}s_ast_statement;

typedef struct s_ast_statement_list
{
    s_ast_statement*    statement;
    struct s_ast_statement_list* statement_list;
}s_ast_statement_list;

typedef struct
{
    s_ast_statement_list* statement_list;
}s_ast_compound_statement_main;

typedef struct
{
	s_ast_declarations* declarations;
	s_ast_compound_statement_main* compound_statement_main;
}s_ast_program;

/*****************************/

#endif /* INTERPRETER_AST_H_ */
