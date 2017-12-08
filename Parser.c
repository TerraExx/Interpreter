/*
 * Parser.c
 *
 *  Created on: Dec 2, 2017
 *      Author: Josip
 */

#include "Common.h"
#include "Lexer.h"
#include "AST.h"
#include "Parser.h"

void Parser_Init(s_parser_parser* Parser, s_lexer_lexer* Lexer)
{
	Parser->lexer = Lexer;
	Parser->current_token = Lexer_GetNextToken(Lexer);
}

s_lexer_token Parser_Eat(s_parser_parser* Parser, e_lexer_token_type Type)
{
	s_lexer_token Token;

	if(Parser->current_token.type == Type)
	{
		Parser->current_token = Lexer_GetNextToken(Parser->lexer);
	}
	else
	{
		printf("Wrong Token Type -> Current_Token: %d, Expected: %d.\n", Parser->current_token.type, Type);
		exit(1);
	}

	return Token;
}

s_ast_type_spec* Parser_Type_Spec(s_parser_parser* Parser)
{
	/*   type_spec : INT8 | INT16 | INT32 | INT8 | INT16 | INT32 | FLOAT   */
	s_ast_type_spec*	Type_SpecNode;

	Type_SpecNode = (s_ast_type_spec*) malloc(sizeof(s_ast_type_spec));

    Type_SpecNode->token = Parser->current_token;

	if(Parser->current_token.type == UINT8)
	{
		Parser_Eat(Parser, UINT8);
	}
	else if(Parser->current_token.type == UINT16)
	{
		Parser_Eat(Parser, UINT16);
	}
	else if(Parser->current_token.type == UINT32)
	{
		Parser_Eat(Parser, UINT32);
	}
	else if(Parser->current_token.type == INT8)
	{
		Parser_Eat(Parser, INT8);
	}
	else if(Parser->current_token.type == INT16)
	{
		Parser_Eat(Parser, INT16);
	}
	else if(Parser->current_token.type == INT32)
	{
		Parser_Eat(Parser, INT32);
	}
	else if(Parser->current_token.type == FLOAT)
	{
		Parser_Eat(Parser, FLOAT);
	}
	else
	{
		printf("Unknown type: %s", Parser->current_token.value.string);
		exit(1);
	}

	return Type_SpecNode;
}

s_ast_factor* Parser_Factor(s_parser_parser* Parser)
{
	/*   factor : PLUS factor | MINUS factor | INTEGER_CONST | REAL_CONST | LPAREN expr RPAREN | ID   */
	s_ast_factor*	FactorNode;

	FactorNode = (s_ast_factor*) malloc(sizeof(s_ast_factor));

    FactorNode->token = Parser->current_token;

	if(Parser->current_token.type == ID)
	{
		Parser_Eat(Parser, ID);
	}
	else
	{
		printf("Incompatible variable name: %s", Parser->current_token.value.string);
		exit(1);
	}

	return FactorNode;
}

s_ast_variable_declaration* Parser_Variable_Declaration(s_parser_parser* Parser)
{
	/*   variable_declaration : type_spec ID (COMMA ID)* SEMICOL    */
	s_ast_variable_declaration* VariableDeclarationNode;
	s_ast_factor_link**         TempFactorLinkPtr;

	VariableDeclarationNode = (s_ast_variable_declaration*) malloc(sizeof(s_ast_variable_declaration));

	VariableDeclarationNode->type_spec = Parser_Type_Spec(Parser);

	VariableDeclarationNode->factor_head_link = NULL;

	TempFactorLinkPtr = &VariableDeclarationNode->factor_head_link;

	/* Parse the name of the first variable */
	(*TempFactorLinkPtr) = (s_ast_factor_link*) malloc(sizeof(s_ast_factor_link));
	(*TempFactorLinkPtr)->factor = Parser_Factor(Parser);

	(*TempFactorLinkPtr)->next_factor_link = NULL;
	TempFactorLinkPtr = &(*TempFactorLinkPtr)->next_factor_link;

	/* Continue parsing variable names */
	while(Parser->current_token.type == COMMA)
	{
        Parser_Eat(Parser, COMMA);

        (*TempFactorLinkPtr) = (s_ast_factor_link*) malloc(sizeof(s_ast_factor_link));
        (*TempFactorLinkPtr)->factor = Parser_Factor(Parser);

        (*TempFactorLinkPtr)->next_factor_link = NULL;
        TempFactorLinkPtr = &(*TempFactorLinkPtr)->next_factor_link;
	}

	Parser_Eat(Parser, SEMICOL);

	return VariableDeclarationNode;
}

s_ast_declarations* Parser_Declarations(s_parser_parser* Parser)
{
	/*   declarations : VARIABLES LCURLY variable_declaration* RCULRY    */

    s_ast_declarations* DeclarationsNode;
	s_ast_variable_declaration_link** TempPtr;

	DeclarationsNode = (s_ast_declarations*) malloc(sizeof(s_ast_declarations));
	DeclarationsNode->variable_declaration_head_link = NULL;

	Parser_Eat(Parser, VARIABLES);
	Parser_Eat(Parser, LCURLY);

	TempPtr = &DeclarationsNode->variable_declaration_head_link;
	while( (Parser->current_token.type >= INT8) && (Parser->current_token.type <= FLOAT) )
	{
	    (*TempPtr) = (s_ast_variable_declaration_link*) malloc(sizeof(s_ast_variable_declaration_link));

	    (*TempPtr)->variable_declaration = Parser_Variable_Declaration(Parser);

	    (*TempPtr)->next_variable_declaration_link = NULL;
		TempPtr = &(*TempPtr)->next_variable_declaration_link;
	}

	Parser_Eat(Parser, RCURLY);

	return DeclarationsNode;
}

s_ast_statement_list* Parser_Statement_List(s_parser_parser* Parser)
{
    /*   statement_list : statement | statement SEMI statement_list   */

    s_ast_statement_list*   StatementListNode;
    s_ast_statement_list**  TempStatementListNodePtr;

    TempStatementListNodePtr = &StatementListNode;

    (*TempStatementListNodePtr) = NULL;

    while(Parser->current_token.type != RCURLY)
    {
        (*TempStatementListNodePtr)->statement_list = (s_ast_statement_list*) malloc(sizeof(s_ast_statement_list));

//        (*TempStatementListNodePtr)->statement = Parser_Statement( Parser);

        TempStatementListNodePtr = (*TempStatementListNodePtr)->statement_list->statement_list;

        (*TempStatementListNodePtr) = NULL;
    }

    return StatementListNode;
}

s_ast_compound_statement_main* Parser_Compound_Statement_Main(s_parser_parser* Parser)
{
    /*   compound_statement_main : MAIN  LCURLY statement_list RCURLY   */

    s_ast_compound_statement_main* CompoundStatementMainNode;

    CompoundStatementMainNode = (s_ast_compound_statement_main*) malloc(sizeof(s_ast_compound_statement_main));

    Parser_Eat(Parser, MAIN);
    Parser_Eat(Parser, LCURLY);

    CompoundStatementMainNode->statement_list = Parser_Statement_List(Parser);

    Parser_Eat(Parser, RCURLY);

    return CompoundStatementMainNode;
}

s_ast_program* Parser_Parse(s_parser_parser* Parser)
{
	/*   program : declarations compound_statement_main   */

	s_ast_program* ProgramNode;

	ProgramNode = (s_ast_program*) malloc(sizeof(s_ast_program));

	ProgramNode->declarations = Parser_Declarations(Parser);

	return ProgramNode;
}
