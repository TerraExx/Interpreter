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

s_ast_compound_main* Parser_Compound_Statement_Main(s_parser_parser* Parser)
{
    /*   compound_statement_main : MAIN  LCURLY statement_list RCURLY   */

	s_ast_compound_main* CompoundMainNode;

	CompoundMainNode = (s_ast_compound_main*) malloc(sizeof(s_ast_compound_main));

	/* Init */
	CompoundMainNode->info.type = COMPOUND_MAIN;

	CompoundMainNode->statement_link.statement = NULL;
	CompoundMainNode->statement_link.next_statement_link = NULL;
	CompoundMainNode->statement_link.perv_statement_link = NULL;

    Parser_Eat(Parser, MAIN);
    Parser_Eat(Parser, LCURLY);

    /* Parse Statements */

    Parser_Eat(Parser, RCURLY);

    return CompoundMainNode;
}

/********************************   VARIABLE DECLARATION START   ********************************/

s_ast_variable* Parser_Variable(s_parser_parser* Parser)
{
  /*   variable : ID   */

  s_ast_variable* VariableNode;

  VariableNode = (s_ast_variable*) malloc(sizeof(s_ast_variable));

  /* Init */
  VariableNode->info.type = VAR;
  VariableNode->token = Parser->current_token;

  Parser_Eat(Parser, ID);

  return VariableNode;
}

s_ast_type* Parser_Type_Spec(s_parser_parser* Parser)
{
  /*   type_spec : INT8 | INT16 | INT32 | INT8 | INT16 | INT32 | FLOAT   */
    s_ast_type*    Type_SpecNode;

  Type_SpecNode = (s_ast_type*) malloc(sizeof(s_ast_type));

  /* Init */
  Type_SpecNode->info.type = TYPE;
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

s_ast_var_declaration* Parser_Declaration(s_parser_parser* Parser)
{
  /*   declarations : VARIABLES LCURLY variable_declaration* RCULRY    */

  s_ast_var_declaration* DeclarationNode;
  struct var_link*       VarLinkPtr;
  struct var_link*       TempPtr;

  DeclarationNode = (s_ast_var_declaration*) malloc(sizeof(s_ast_var_declaration));

  /* Init */
  DeclarationNode->info.type = VAR_DECL;

  DeclarationNode->type = NULL;

  DeclarationNode->var_link.variable = NULL;
  DeclarationNode->var_link.next_var_link = NULL;
  DeclarationNode->var_link.prev_var_link = NULL;


  /* Parse variable declaration */
  DeclarationNode->type = Parser_Type_Spec( Parser );

  VarLinkPtr = &DeclarationNode->var_link;

  /* Parser variable names */
  for(; ; Parser_Eat(Parser, COMMA))
  {
      VarLinkPtr->variable = Parser_Variable( Parser );

      VarLinkPtr->next_var_link = (struct var_link*) malloc(sizeof(struct var_link));

      /* Init new link */
      TempPtr = VarLinkPtr;

      VarLinkPtr = VarLinkPtr->next_var_link;

      VarLinkPtr->variable = NULL;
      VarLinkPtr->next_var_link = NULL;
      VarLinkPtr->prev_var_link = TempPtr;

      /* Check if that was the last variable */
      if(Parser->current_token.type == SEMICOL)
      {
    	  break;
      }
  }

  /* Free/DeInit the last allocated link */
  TempPtr = VarLinkPtr->prev_var_link;
  TempPtr->next_var_link = NULL;
  free(VarLinkPtr);

  Parser_Eat(Parser, SEMICOL);

  return DeclarationNode;
}

s_ast_program* Parser_Parse(s_parser_parser* Parser)
{
	/*   program : declarations? compound_statement_main   */

	s_ast_program*          ProgramNode;
	struct var_decl_link*   VarDeclLinkPtr;
    struct var_decl_link*   TempPtr;

	ProgramNode = (s_ast_program*) malloc(sizeof(s_ast_program));

	/* Init */
	ProgramNode->info.type = PROGRAM;

	ProgramNode->var_decl_link.var_declaration = NULL;
	ProgramNode->var_decl_link.next_var_decl_link = NULL;
	ProgramNode->var_decl_link.perv_var_decl_link = NULL;

	ProgramNode->compound_main = NULL;


	/* Parse global variable declarations */
	if(Parser->current_token.type == VARIABLES)
	{
	    Parser_Eat(Parser, VARIABLES);
	    Parser_Eat(Parser, LCURLY);

	    VarDeclLinkPtr = &ProgramNode->var_decl_link;

	    while(Parser->current_token.type != RCURLY)
	    {
	        VarDeclLinkPtr->var_declaration = Parser_Declaration( Parser );

	        VarDeclLinkPtr->next_var_decl_link = (struct var_decl_link*) malloc(sizeof(struct var_decl_link));

	        /* Init new link */
	        TempPtr = VarDeclLinkPtr;

	        VarDeclLinkPtr = VarDeclLinkPtr->next_var_decl_link;

	        VarDeclLinkPtr->var_declaration = NULL;
	        VarDeclLinkPtr->next_var_decl_link = NULL;
	        VarDeclLinkPtr->perv_var_decl_link = TempPtr;
	    }

	    /* Free/DeInit the last allocated link */
	    TempPtr = VarDeclLinkPtr->perv_var_decl_link;
	    TempPtr->next_var_decl_link = NULL;
	    free(VarDeclLinkPtr);

	    Parser_Eat(Parser, RCURLY);
	}

	/* Parse Compound Main */
	ProgramNode->compound_main = Parser_Compound_Statement_Main( Parser );

	return ProgramNode;
}
