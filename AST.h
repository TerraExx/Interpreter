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

typedef struct
{
	s_ast_type_spec*	type_spec;
	s_ast_factor*		id;
}s_ast_variable_declaration;

typedef struct
{
	s_ast_variable_declaration*	variable_declaration;
	void* next_variable_declaration_link;
}s_ast_variable_declaration_link;

typedef struct
{

}s_ast_compound_statement_main;

typedef struct
{
	s_ast_variable_declaration_link* declarations;
	s_ast_compound_statement_main* compound_statement_main;
}s_ast_program;

/*****************************/

#endif /* INTERPRETER_AST_H_ */
