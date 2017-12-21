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

//s_ast_factor* Parser_Factor(s_parser_parser* Parser)
//{
//	/*   factor : PLUS factor | MINUS factor | INTEGER_CONST | REAL_CONST | LPAREN expr RPAREN | variable   */
//	s_ast_factor*	FactorNode;
//
//	FactorNode = (s_ast_factor*) malloc(sizeof(s_ast_factor));
//
//    FactorNode->token = Parser->current_token;
//
//	if(Parser->current_token.type == ID)
//	{
//		Parser_Eat(Parser, ID);
//	}
//	else
//	{
//		printf("Incompatible variable name: %s", Parser->current_token.value.string);
//		exit(1);
//	}
//
//	return FactorNode;
//}
//
//s_ast_variable_declaration* Parser_Variable_Declaration(s_parser_parser* Parser)
//{
//	/*   variable_declaration : type_spec ID (COMMA ID)* SEMICOL    */
//	s_ast_variable_declaration* VariableDeclarationNode;
//	s_ast_variable_link**         TempFactorLinkPtr;
//
//	VariableDeclarationNode = (s_ast_variable_declaration*) malloc(sizeof(s_ast_variable_declaration));
//
//	VariableDeclarationNode->type_spec = Parser_Type_Spec(Parser);
//
//	VariableDeclarationNode->variable_head_link = NULL;
//
//	TempFactorLinkPtr = &VariableDeclarationNode->variable_head_link;
//
//	/* Parse the name of the first variable */
//	(*TempFactorLinkPtr) = (s_ast_variable_link*) malloc(sizeof(s_ast_variable_link));
//	(*TempFactorLinkPtr)->variable = Parser_Variable(Parser);
//
//	(*TempFactorLinkPtr)->next_variable_link = NULL;
//	TempFactorLinkPtr = &(*TempFactorLinkPtr)->next_variable_link;
//
//	/* Continue parsing variable names */
//	while(Parser->current_token.type == COMMA)
//	{
//        Parser_Eat(Parser, COMMA);
//
//        (*TempFactorLinkPtr) = (s_ast_variable_link*) malloc(sizeof(s_ast_variable_link));
//        (*TempFactorLinkPtr)->variable = Parser_Variable(Parser);
//
//        (*TempFactorLinkPtr)->next_variable_link = NULL;
//        TempFactorLinkPtr = &(*TempFactorLinkPtr)->next_variable_link;
//	}
//
//	Parser_Eat(Parser, SEMICOL);
//
//	return VariableDeclarationNode;
//}
//
//
//s_ast_assignment* Parser_Assignment(s_parser_parser* Parser)
//{
//	/*   assignment_statement : ID ASSIGN expr   */
//
//	s_ast_assignment*	AssignmentNode;
//
//	AssignmentNode = (s_ast_assignment*) malloc(sizeof(s_ast_assignment));
//
//	AssignmentNode->variable = Parser_Variable( Parser );
//
//	Parser_Eat(Parser, ASSIGN);
//
//
//
//	return AssignmentNode;
//}
//
//s_ast_statement* Parser_Statement(s_parser_parser* Parser)
//{
//	/*   statement : assignment_statement | empty   */
//
//	s_ast_statement*	StatementNode;
//
//	StatementNode = (s_ast_statement*) malloc(sizeof(s_ast_statement));
//
//	if(Parser->current_token.type == ID)
//	{
//		StatementNode->type = ASSIGNMENT;
//
//	}
//
//	return StatementNode;
//}
//
//s_ast_statement_list* Parser_Statement_List(s_parser_parser* Parser)
//{
//    /*   statement_list : statement | statement SEMI statement_list   */
//
//    s_ast_statement_list*   StatementListNode;
//    s_ast_statement_list**  TempStatementListNodePtr;
//
//    TempStatementListNodePtr = &StatementListNode;
//
//    (*TempStatementListNodePtr) = NULL;
//
//    while(Parser->current_token.type != RCURLY)
//    {
//        (*TempStatementListNodePtr) = (s_ast_statement_list*) malloc(sizeof(s_ast_statement_list));
//
//        (*TempStatementListNodePtr)->statement = Parser_Statement( Parser);
//
//        TempStatementListNodePtr = &(*TempStatementListNodePtr)->statement_list;
//
//        (*TempStatementListNodePtr) = NULL;
//    }
//
//    return StatementListNode;
//}
//
//s_ast_compound_statement_main* Parser_Compound_Statement_Main(s_parser_parser* Parser)
//{
//    /*   compound_statement_main : MAIN  LCURLY statement_list RCURLY   */
//
//    s_ast_compound_statement_main* CompoundStatementMainNode;
//
//    CompoundStatementMainNode = (s_ast_compound_statement_main*) malloc(sizeof(s_ast_compound_statement_main));
//
//    Parser_Eat(Parser, MAIN);
//    Parser_Eat(Parser, LCURLY);
//
//    CompoundStatementMainNode->statement_list = Parser_Statement_List(Parser);
//
//    Parser_Eat(Parser, RCURLY);
//
//    return CompoundStatementMainNode;
//}

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

  do
  {
      VarLinkPtr->variable = Parser_Declaration( Parser );

      VarLinkPtr->next_var_link = (struct var_link*) malloc(sizeof(struct var_link));

      /* Init new link */
      TempPtr = VarLinkPtr;

      VarLinkPtr = VarLinkPtr->next_var_link;

      VarLinkPtr->variable = NULL;
      VarLinkPtr->next_var_link = NULL;
      VarLinkPtr->prev_var_link = TempPtr;
  }while(Parser->current_token.type == COMMA);

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

	return ProgramNode;
}
