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
	COMPOUND_RETURN,
	PARAMETER,
	PROGRAM
}e_ast_node_type;

/********* AST Nodes *********/

/* Default AST node info */
typedef struct
{
	e_ast_node_type    type;
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

/* Compound Return */
typedef struct
{
    s_ast_node_info info;
    
    s_ast_type*   type;
} s_ast_compound_return;

/* Parameter */
typedef struct
{
    s_ast_node_info info;

    s_ast_type*     type;
    s_ast_variable* variable;
} s_ast_parameter;

/* Compound */
typedef struct
{
    s_ast_node_info info;

    /* Return Type */
    s_ast_compound_return*  return_type;

    /* Name */
    s_lexer_token   name;

    /* Parameter linked list */
    struct parameter_link
    {
        s_ast_parameter*        parameter;
        struct parameter_link*  next_parameter_link;
        struct parameter_link*  perv_parameter_link;
    } parameter_link;

    /* Statement linked list */
    struct c_statement_link
    {
        void*                   statement;
        struct c_statement_link*  next_statement_link;
        struct c_statement_link*  perv_statement_link;
    } statement_link;

} s_ast_compound;

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

	/* Compound Statement linked list */
    struct compound_link
    {
	    s_ast_compound* compound_statement;
        struct compound_link*  next_compound_link;
        struct compound_link*  perv_compound_link;
    } compound_link;

	s_ast_compound_main* compound_main;

} s_ast_program;

/*****************************/

#endif /* INTERPRETER_AST_H_ */
