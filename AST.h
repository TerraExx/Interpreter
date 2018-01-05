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
	COMPOUND_CALL,
	ARGUMENT,
	RETURN_STATEMENT,
	IF_STATEMENT,
	IF_CONDITION,
	ELSE_CONDITION,
	PROGRAM
}e_ast_node_type;

/********* AST Nodes *********/

/* Default AST node info */
typedef struct
{
	e_ast_node_type    type;
	uint16_t           line;
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

/* Statement linked list link */
typedef struct s_ast_statement_link
{
    void*                         statement;
    struct s_ast_statement_link*  next_statement_link;
    struct s_ast_statement_link*  perv_statement_link;
} s_ast_statement_link;

/* If Condition */
typedef struct
{
    s_ast_node_info info;

    void*   condition;

    /* If Statement linked list */
    s_ast_statement_link statement_link;
} s_ast_ifCondition;

/* Else Condition */
typedef struct
{
    s_ast_node_info info;

    /* Else Statement linked list */
    s_ast_statement_link statement_link;
} s_ast_elseCodnition;

/* If Statement */
typedef struct
{
    s_ast_node_info info;

    /* if Condition linked list */
    struct if_condition_link
    {
        s_ast_ifCondition*         ifCondition;
        struct if_condition_link*  next_ifCondition_link;
        struct if_condition_link*  perv_ifCondition_link;
    } if_condition_link;

    s_ast_elseCodnition* elseCondition;
} s_ast_ifStatement;

/* Compound Return Statement */
typedef struct
{
    s_ast_node_info info;
    void* value;
} s_ast_compoundReturnStatement;

/* Compound Call Argument */
typedef struct
{
    s_ast_node_info info;
    void* argument;
} s_ast_compoundCallArg;

/* Compound Statement Call */
typedef struct
{
    s_ast_node_info info;
    s_lexer_token   cmpStatementName;

    /* Compound Statement Call Argument linked list */
    struct argument_link
    {
        s_ast_compoundCallArg* argument;
        struct argument_link*  next_argument_link;
        struct argument_link*  perv_argument_link;
    } argument_link;

} s_ast_compoundStatementCall;

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

    /* Compound Variable Declarations linked list */
    struct cmp_var_decl_link
    {
        s_ast_var_declaration* var_declaration;
        struct cmp_var_decl_link*  next_var_decl_link;
        struct cmp_var_decl_link*  perv_var_decl_link;
    } cmp_var_decl_link;

    /* Statement linked list */
    s_ast_statement_link statement_link;

} s_ast_compound;

/* Compound Main */
typedef struct
{
	s_ast_node_info	info;

    /* Main Variable Declarations linked list */
    struct main_var_decl_link
    {
        s_ast_var_declaration* var_declaration;
        struct main_var_decl_link*  next_var_decl_link;
        struct main_var_decl_link*  perv_var_decl_link;
    } main_var_decl_link;

	/* Statement linked list */
	s_ast_statement_link statement_link;

} s_ast_compound_main;

/* Program */
typedef struct
{
	s_ast_node_info	info;

	/* Global Variable Declarations linked list */
	struct global_var_decl_link
	{
		s_ast_var_declaration* var_declaration;
		struct global_var_decl_link*  next_var_decl_link;
		struct global_var_decl_link*  perv_var_decl_link;
	} global_var_decl_link;

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
