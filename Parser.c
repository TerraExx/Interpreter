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

	Type_SpecNode->token = Parser->current_token;

	return Type_SpecNode;
}

s_ast_factor* Parser_Factor(s_parser_parser* Parser)
{
	/*   factor : PLUS factor | MINUS factor | INTEGER_CONST | REAL_CONST | LPAREN expr RPAREN | ID   */
	s_ast_factor*	FactorNode;

	FactorNode = (s_ast_factor*) malloc(sizeof(s_ast_factor));

	if(Parser->current_token.type == ID)
	{
		Parser_Eat(Parser, ID);
	}
	else
	{
		printf("Incompatible variable name: %s", Parser->current_token.value.string);
		exit(1);
	}

	FactorNode->token = Parser->current_token;

	return FactorNode;
}

s_ast_variable_declaration* Parser_Variable_Declaration(s_parser_parser* Parser)
{
	/*   variable_declaration : type_spec ID (COMMA ID)* SEMICOL    */
	s_ast_variable_declaration* DeclarationNode;

	DeclarationNode = (s_ast_variable_declaration*) malloc(sizeof(s_ast_variable_declaration));

	DeclarationNode->type_spec = Parser_Type_Spec(Parser);
	DeclarationNode->id = Parser_Factor(Parser);

	Parser_Eat(Parser, SEMICOL);

	return DeclarationNode;
}

s_ast_variable_declaration_link* Parser_Declarations(s_parser_parser* Parser)
{
	/*   declarations : VARIABLES LCURLY variable_declaration* RCULRY    */

	s_ast_variable_declaration_link* FirstDeclarationNode;
	s_ast_variable_declaration_link* TempPtr;

	FirstDeclarationNode = (s_ast_variable_declaration_link*) malloc(sizeof(s_ast_variable_declaration_link));
	FirstDeclarationNode->variable_declaration = NULL;
	FirstDeclarationNode->next_variable_declaration_link = NULL;

	Parser_Eat(Parser, VARIABLES);
	Parser_Eat(Parser, LCURLY);

	TempPtr = FirstDeclarationNode;
	while( (Parser->current_token.type >= INT8) && (Parser->current_token.type <= FLOAT) )
	{
		TempPtr->variable_declaration = Parser_Variable_Declaration(Parser);

		TempPtr->next_variable_declaration_link = (s_ast_variable_declaration_link*) malloc(sizeof(s_ast_variable_declaration_link));
		TempPtr = TempPtr->next_variable_declaration_link;

		TempPtr->variable_declaration = NULL;
		TempPtr->next_variable_declaration_link = NULL;
	}

	Parser_Eat(Parser, RCURLY);

	return FirstDeclarationNode;
}

s_ast_program Parser_Parse(s_parser_parser* Parser)
{
	/*   program : declarations compound_statement_main   */

	s_ast_program ProgramNode;

	ProgramNode.declarations = Parser_Declarations(Parser);


	return ProgramNode;
}
