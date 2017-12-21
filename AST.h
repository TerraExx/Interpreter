/*
 * AST.h
 *
 *  Created on: Dec 2, 2017
 *      Author: Josip
 */

#ifndef INTERPRETER_AST_H_
#define INTERPRETER_AST_H_

/********* Statement types *********/

typedef enum
{
	NO_OP,
	TYPE,
	NUM,
	VAR,
	UNARY_OP,
	BINARY_OP,
	ASSIGNMENT,
	VAR_DECL,
	COMPOUND_MAIN,
	COMPOUND,
	PROGRAM
}e_ast_statement_type;

/********* AST Nodes *********/

/* Default AST node info */
typedef struct
{
	e_ast_statement_type    type;
} s_ast_node_info;

/* No Operation */
typedef struct
{
	s_ast_node_info	info;
} s_ast_noop;

/* Type */
typedef struct
{
	s_ast_node_info	info;
	s_lexer_token	token;
} s_ast_type;

/* Numerical */
typedef struct
{
	s_ast_node_info	info;
	s_lexer_token	token;
} s_ast_num;

/* Variable */
typedef struct
{
	s_ast_node_info	info;
	s_lexer_token	token;
} s_ast_variable;

/* Unary Operation */
typedef struct
{
	s_ast_node_info	info;
	s_lexer_token	token;
	void*			operand;
} s_ast_unary;

/* Binary Operation */
typedef struct
{
	s_ast_node_info	info;
	s_lexer_token	token;
	void*			leftOperand;
	void*			rightOperand;
} s_ast_binary;

/* Assignment */
typedef struct
{
	s_ast_node_info	info;
	s_ast_variable* variable;
	void*			expression;
} s_ast_assignment;

/* Variable Declaration */
typedef struct
{
	s_ast_node_info	info;
	s_ast_type*		type;

	/* Variable linked list */
	struct var_link
	{
		s_ast_variable*	variable;
		struct var_link*	next_var_link;
		struct var_link*	prev_var_link;
	} var_link;

} s_ast_var_declaration;

/* Compound Main */
typedef struct
{
	s_ast_node_info	info;

	/* Statement linked list */
	struct statement_link
	{
		void*					statement;
		struct statement_link*	next_statement_link;
		struct statement_link*	perv_statement_link;
	} statement_link;

} s_ast_compound_main;

/* Program */
typedef struct
{
	s_ast_node_info	info;

	/* Variable Declarations linked list */
	struct var_decl_link
	{
		s_ast_var_declaration* var_declaration;
		struct var_decl_link*  next_var_decl_link;
		struct var_decl_link*  perv_var_decl_link;
	} var_decl_link;

	s_ast_compound_main* compound_main;

} s_ast_program;

/*****************************/

#endif /* INTERPRETER_AST_H_ */