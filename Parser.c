/*
 * Parser.c
 *
 *  Created on: Dec 2, 2017
 *      Author: Josip
 */

#include "System.h"
#include "Lexer.h"
#include "AST.h"
#include "Parser.h"

/* Prototypes */
void* Parser_Expression(s_parser_parser* Parser);

/* Function Implementation */

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
	    System_Print("Parser Error: Wrong Token Type -> Current_Token: %d, Expected: %d.\n", Parser->current_token.type, Type);
		exit(1);
	}

	return Token;
}

s_ast_type* Parser_Type_Spec(s_parser_parser* Parser)
{
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
      System_Print("Parser Error: \nLine %d:\nUnknown type: %s\n", Parser->current_token.line, Parser->current_token.value.string);
      exit(1);
  }

  return Type_SpecNode;
}

s_ast_variable* Parser_Variable(s_parser_parser* Parser)
{
  s_ast_variable* VariableNode;

  VariableNode = (s_ast_variable*) malloc(sizeof(s_ast_variable));

  /* Init */
  VariableNode->info.type = VAR;
  VariableNode->token = Parser->current_token;

  Parser_Eat(Parser, ID);

  return VariableNode;
}

void* Parser_Factor(s_parser_parser* Parser)
{
    void*           Factor;
    s_ast_unary*    UnaryOpNode;
    s_ast_num*      NumNode;
    s_ast_variable* VariableNode;


    /* Init */
    Factor = NULL;
    UnaryOpNode = NULL;
    NumNode = NULL;
    VariableNode = NULL;

    if(Parser->current_token.type == PLUS)
    {
        UnaryOpNode = (s_ast_unary*) malloc(sizeof(s_ast_unary));

        /* Init */
        UnaryOpNode->info.type = UNARY_OP;
        UnaryOpNode->token = Parser->current_token;

        Parser_Eat( Parser, PLUS );

        UnaryOpNode->operand = Parser_Factor( Parser );

        Factor = UnaryOpNode;
    }
    else if(Parser->current_token.type == MINUS)
    {
        UnaryOpNode = (s_ast_unary*) malloc(sizeof(s_ast_unary));

        /* Init */
        UnaryOpNode->info.type = UNARY_OP;
        UnaryOpNode->token = Parser->current_token;

        Parser_Eat( Parser, MINUS );

        UnaryOpNode->operand = Parser_Factor( Parser );

        Factor = UnaryOpNode;
    }
    else if(Parser->current_token.type == INTEGER_CONST)
    {
        NumNode = (s_ast_num*) malloc(sizeof(s_ast_num));

        /* Init */
        NumNode->info.type = NUM;

        NumNode->token = Parser->current_token;

        Parser_Eat( Parser, INTEGER_CONST );

        Factor = NumNode;
    }
    else if(Parser->current_token.type == REAL_CONST)
    {
        /* Float not implemented yet */
    }
    else if(Parser->current_token.type == LPAREN)
    {
        Parser_Eat( Parser, LPAREN );

        Factor = Parser_Expression( Parser );

        Parser_Eat( Parser, RPAREN );
    }
    else if(Parser->current_token.type == ID)
    {
        VariableNode = (s_ast_variable*) malloc(sizeof(s_ast_variable));

        /* Init */
        VariableNode->info.type = VAR;

        VariableNode->token = Parser->current_token;

        Parser_Eat( Parser, ID );

        Factor = VariableNode;
    }

    return Factor;
}

void* Parser_Term(s_parser_parser* Parser)
{
    void*   Term;
    s_ast_binary*   BinaryOpNode;

    /* Init */
    Term = NULL;

    Term = Parser_Factor( Parser );

    while(Parser->current_token.type == MUL || Parser->current_token.type == DIV)
    {
        BinaryOpNode = (s_ast_binary*) malloc(sizeof(s_ast_binary));

        /* Init */
        BinaryOpNode->info.type = BINARY_OP;
        BinaryOpNode->token = Parser->current_token;

        if(Parser->current_token.type == MUL)
        {
            Parser_Eat( Parser, MUL );
        }
        else if(Parser->current_token.type == DIV)
        {
            Parser_Eat( Parser, DIV );
        }

        BinaryOpNode->leftOperand = Term;
        BinaryOpNode->rightOperand = Parser_Factor( Parser );

        Term = (void*)BinaryOpNode;
    }

    return Term;
}

void* Parser_Expression(s_parser_parser* Parser)
{
    void*           Expression;
    s_ast_binary*   BinaryOpNode;

    /* Init */
    Expression = NULL;
    BinaryOpNode = NULL;

    /* Parse Term */
    Expression = Parser_Term( Parser );

    while(Parser->current_token.type == PLUS || Parser->current_token.type == MINUS)
    {
        BinaryOpNode = (s_ast_binary*) malloc(sizeof(s_ast_binary));

        /* Init */
        BinaryOpNode->info.type = BINARY_OP;
        BinaryOpNode->token = Parser->current_token;

        if(Parser->current_token.type == PLUS)
        {
            Parser_Eat( Parser, PLUS );
        }
        else if(Parser->current_token.type == MINUS)
        {
            Parser_Eat( Parser, MINUS );
        }

        BinaryOpNode->leftOperand = Expression;
        BinaryOpNode->rightOperand = Parser_Term( Parser );

        Expression = (void*)BinaryOpNode;
    }

    return Expression;
}

s_ast_var_declaration* Parser_Declaration(s_parser_parser* Parser)
{
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
  if(VarLinkPtr->prev_var_link != NULL)
  {
      TempPtr = VarLinkPtr->prev_var_link;
      TempPtr->next_var_link = NULL;
      free(VarLinkPtr);
  }

  Parser_Eat(Parser, SEMICOL);

  return DeclarationNode;
}

void* Parser_Statement(s_parser_parser* Parser)
{
    void*   Statement;

    /* Init */
    Statement = NULL;

    /* Check statement type */
    if(Parser->current_token.type == ID)
    {
        /* Assigment Statement */
        Statement = malloc(sizeof(s_ast_assignment));

        /* Init */
        ((s_ast_assignment*)Statement)->info.type = ASSIGNMENT;

        /* Parse variable (left operand) */
        ((s_ast_assignment*)Statement)->variable = Parser_Variable( Parser );

        Parser_Eat( Parser, ASSIGN );

        /* Parse right side (expression) */
        ((s_ast_assignment*)Statement)->expression = Parser_Expression( Parser );

        Parser_Eat( Parser, SEMICOL );
    }

    return Statement;
}

s_ast_compound_main* Parser_Compound_Statement_Main(s_parser_parser* Parser)
{
	s_ast_compound_main* 		 CompoundMainNode;

    struct main_var_decl_link*   VarDeclLinkPtr;
    struct main_var_decl_link*   TempVarDeclPtr;

	struct statement_link*       StatementLinkPtr;
	struct statement_link*       TempPtr;

	CompoundMainNode = (s_ast_compound_main*) malloc(sizeof(s_ast_compound_main));

	/* Init */
	CompoundMainNode->info.type = COMPOUND_MAIN;

	CompoundMainNode->main_var_decl_link.var_declaration = NULL;
	CompoundMainNode->main_var_decl_link.next_var_decl_link = NULL;
	CompoundMainNode->main_var_decl_link.perv_var_decl_link = NULL;

	CompoundMainNode->statement_link.statement = NULL;
	CompoundMainNode->statement_link.next_statement_link = NULL;
	CompoundMainNode->statement_link.perv_statement_link = NULL;

    Parser_Eat(Parser, MAIN);
    Parser_Eat(Parser, LCURLY);

    StatementLinkPtr = &CompoundMainNode->statement_link;

    /* Parse main variable declarations */
    if(Parser->current_token.type >= INT8 && Parser->current_token.type <= FLOAT)
    {
        VarDeclLinkPtr = &CompoundMainNode->main_var_decl_link;

        while(Parser->current_token.type >= INT8 && Parser->current_token.type <= FLOAT)
        {
            VarDeclLinkPtr->var_declaration = Parser_Declaration( Parser );

            VarDeclLinkPtr->next_var_decl_link = (struct main_var_decl_link*) malloc(sizeof(struct main_var_decl_link));

            /* Init new link */
            TempVarDeclPtr = VarDeclLinkPtr;

            VarDeclLinkPtr = VarDeclLinkPtr->next_var_decl_link;

            VarDeclLinkPtr->var_declaration = NULL;
            VarDeclLinkPtr->next_var_decl_link = NULL;
            VarDeclLinkPtr->perv_var_decl_link = TempVarDeclPtr;
        }

        /* Free/DeInit the last allocated link */
        if(VarDeclLinkPtr->perv_var_decl_link != NULL)
        {
            TempVarDeclPtr = VarDeclLinkPtr->perv_var_decl_link;
            TempVarDeclPtr->next_var_decl_link = NULL;
            free(VarDeclLinkPtr);
        }
    }

    /* Parse Statements */
    while(Parser->current_token.type != RCURLY)
    {
    	StatementLinkPtr->statement = Parser_Statement( Parser );

    	StatementLinkPtr->next_statement_link = (struct statement_link*) malloc(sizeof(struct statement_link));

        /* Init new link */
        TempPtr = StatementLinkPtr;

        StatementLinkPtr = StatementLinkPtr->next_statement_link;

        StatementLinkPtr->statement = NULL;
        StatementLinkPtr->next_statement_link = NULL;
        StatementLinkPtr->perv_statement_link = TempPtr;
    }

    /* Free/DeInit the last allocated link */
    if(StatementLinkPtr->perv_statement_link != NULL)
    {
        TempPtr = StatementLinkPtr->perv_statement_link;
        TempPtr->next_statement_link = NULL;
        free(StatementLinkPtr);
    }

    Parser_Eat(Parser, RCURLY);

    return CompoundMainNode;
}

s_ast_parameter* Parser_Compound_Parameter(s_parser_parser* Parser)
{
    s_ast_parameter* CompoundParameterNode;

    CompoundParameterNode = (s_ast_parameter*) malloc(sizeof(s_ast_parameter));

    /* Init */
    CompoundParameterNode->info.type = PARAMETER;

    /* Parse Type */
    CompoundParameterNode->type = Parser_Type_Spec( Parser );

    /* Parse Variable */
    CompoundParameterNode->variable = Parser_Variable( Parser );

    return CompoundParameterNode;
}

s_ast_compound_return* Parser_Compound_Return(s_parser_parser* Parser)
{
    s_ast_compound_return* CompoundReturnNode;

    CompoundReturnNode = (s_ast_compound_return*) malloc(sizeof(s_ast_compound_return));

    /* Init */
    CompoundReturnNode->info.type = COMPOUND_RETURN;

    /* Parse Return Type */
    CompoundReturnNode->type = Parser_Type_Spec( Parser );

    return CompoundReturnNode;
}

s_ast_compound* Parser_Compound_Statement(s_parser_parser* Parser)
{
    s_ast_compound* CoumpoundStatementNode;

    struct parameter_link* ParamLinkPtr;
    struct parameter_link* TempParamPtr;

    struct cmp_var_decl_link*   VarDeclLinkPtr;
    struct cmp_var_decl_link*   TempVarDeclPtr;

    struct c_statement_link* StatementLinkPtr;
    struct c_statement_link* TempStatementPtr;

    CoumpoundStatementNode = (s_ast_compound*) malloc(sizeof(s_ast_compound));

    /* Init */
    CoumpoundStatementNode->info.type = COMPOUND;

    CoumpoundStatementNode->return_type = NULL;

    CoumpoundStatementNode->parameter_link.parameter = NULL;
    CoumpoundStatementNode->parameter_link.next_parameter_link = NULL;
    CoumpoundStatementNode->parameter_link.perv_parameter_link = NULL;

    CoumpoundStatementNode->cmp_var_decl_link.var_declaration = NULL;
    CoumpoundStatementNode->cmp_var_decl_link.next_var_decl_link = NULL;
    CoumpoundStatementNode->cmp_var_decl_link.perv_var_decl_link = NULL;

    CoumpoundStatementNode->statement_link.statement = NULL;
    CoumpoundStatementNode->statement_link.next_statement_link = NULL;
    CoumpoundStatementNode->statement_link.perv_statement_link = NULL;

    /* Parse Return Type */
    CoumpoundStatementNode->return_type = Parser_Compound_Return( Parser );

    /* Parse Name */
    CoumpoundStatementNode->name = Parser->current_token;
    Parser_Eat( Parser, ID );

    /* Parse Parameter List */
    Parser_Eat( Parser, LPAREN );

    ParamLinkPtr = &CoumpoundStatementNode->parameter_link;

    for(; Parser->current_token.type != RPAREN ; Parser_Eat(Parser, COMMA))
    {
        ParamLinkPtr->parameter = Parser_Compound_Parameter( Parser );

        ParamLinkPtr->next_parameter_link = (struct parameter_link*) malloc(sizeof(struct parameter_link));

        /* Init new link */
        TempParamPtr = ParamLinkPtr;

        ParamLinkPtr = ParamLinkPtr->next_parameter_link;

        ParamLinkPtr->parameter = NULL;
        ParamLinkPtr->next_parameter_link = NULL;
        ParamLinkPtr->perv_parameter_link = TempParamPtr;

        if(Parser->current_token.type == RPAREN)
        {
            break;
        }
    }

    /* Free/DeInit the last allocated link */
    if(ParamLinkPtr->perv_parameter_link != NULL)
    {
        TempParamPtr = ParamLinkPtr->perv_parameter_link;
        TempParamPtr->next_parameter_link = NULL;
        free(ParamLinkPtr);
    }

    Parser_Eat( Parser, RPAREN );

    /* Parse Statements */
    Parser_Eat( Parser, LCURLY );

    /* Parse compound variable declarations */
    if(Parser->current_token.type >= INT8 && Parser->current_token.type <= FLOAT)
    {
        VarDeclLinkPtr = &CoumpoundStatementNode->cmp_var_decl_link;

        while(Parser->current_token.type >= INT8 && Parser->current_token.type <= FLOAT)
        {
            VarDeclLinkPtr->var_declaration = Parser_Declaration( Parser );

            VarDeclLinkPtr->next_var_decl_link = (struct cmp_var_decl_link*) malloc(sizeof(struct cmp_var_decl_link));

            /* Init new link */
            TempVarDeclPtr = VarDeclLinkPtr;

            VarDeclLinkPtr = VarDeclLinkPtr->next_var_decl_link;

            VarDeclLinkPtr->var_declaration = NULL;
            VarDeclLinkPtr->next_var_decl_link = NULL;
            VarDeclLinkPtr->perv_var_decl_link = TempVarDeclPtr;
        }

        /* Free/DeInit the last allocated link */
        if(VarDeclLinkPtr->perv_var_decl_link != NULL)
        {
            TempVarDeclPtr = VarDeclLinkPtr->perv_var_decl_link;
            TempVarDeclPtr->next_var_decl_link = NULL;
            free(VarDeclLinkPtr);
        }
    }

    StatementLinkPtr = &CoumpoundStatementNode->statement_link;

    while( Parser->current_token.type != RCURLY )
    {
        StatementLinkPtr->statement = Parser_Statement( Parser );

        StatementLinkPtr->next_statement_link = (struct c_statement_link*) malloc(sizeof(struct c_statement_link));

        /* Init new link */
        TempStatementPtr = StatementLinkPtr;

        StatementLinkPtr = StatementLinkPtr->next_statement_link;

        StatementLinkPtr->statement = NULL;
        StatementLinkPtr->next_statement_link = NULL;
        StatementLinkPtr->perv_statement_link = TempStatementPtr;
    }

    /* Free/DeInit the last allocated link */
    if(StatementLinkPtr->perv_statement_link != NULL)
    {
        TempStatementPtr = StatementLinkPtr->perv_statement_link;
        TempStatementPtr->next_statement_link = NULL;
        free(StatementLinkPtr);
    }

    Parser_Eat( Parser, RCURLY );

    return CoumpoundStatementNode;
}

s_ast_program* Parser_Parse(s_parser_parser* Parser)
{
	s_ast_program*          ProgramNode;

	struct global_var_decl_link*   VarDeclLinkPtr;
    struct global_var_decl_link*   TempVarDeclPtr;

    struct compound_link*   CompoundLinkPtr;
    struct compound_link*   TempCompoundPtr;

	ProgramNode = (s_ast_program*) malloc(sizeof(s_ast_program));

	/* Init */
	ProgramNode->info.type = PROGRAM;

	ProgramNode->global_var_decl_link.var_declaration = NULL;
	ProgramNode->global_var_decl_link.next_var_decl_link = NULL;
	ProgramNode->global_var_decl_link.perv_var_decl_link = NULL;

    ProgramNode->compound_link.compound_statement = NULL;
    ProgramNode->compound_link.next_compound_link = NULL;
    ProgramNode->compound_link.perv_compound_link = NULL;

	ProgramNode->compound_main = NULL;


	/* Parse global variable declarations */
	if(Parser->current_token.type == VARIABLES)
	{
	    Parser_Eat(Parser, VARIABLES);
	    Parser_Eat(Parser, LCURLY);

	    VarDeclLinkPtr = &ProgramNode->global_var_decl_link;

	    while(Parser->current_token.type != RCURLY)
	    {
	        VarDeclLinkPtr->var_declaration = Parser_Declaration( Parser );

	        VarDeclLinkPtr->next_var_decl_link = (struct global_var_decl_link*) malloc(sizeof(struct global_var_decl_link));

	        /* Init new link */
	        TempVarDeclPtr = VarDeclLinkPtr;

	        VarDeclLinkPtr = VarDeclLinkPtr->next_var_decl_link;

	        VarDeclLinkPtr->var_declaration = NULL;
	        VarDeclLinkPtr->next_var_decl_link = NULL;
	        VarDeclLinkPtr->perv_var_decl_link = TempVarDeclPtr;
	    }

	    /* Free/DeInit the last allocated link */
	    if(VarDeclLinkPtr->perv_var_decl_link != NULL)
	    {
	        TempVarDeclPtr = VarDeclLinkPtr->perv_var_decl_link;
	        TempVarDeclPtr->next_var_decl_link = NULL;
	        free(VarDeclLinkPtr);
	    }

	    Parser_Eat(Parser, RCURLY);
	}

	/* Parse Compound_statements (functions) */
	CompoundLinkPtr = &ProgramNode->compound_link;

	while(Parser->current_token.type != MAIN)
	{
	    CompoundLinkPtr->compound_statement = Parser_Compound_Statement( Parser );

	    CompoundLinkPtr->next_compound_link = (struct compound_link*) malloc(sizeof(struct compound_link));

        /* Init new link */
	    TempCompoundPtr = CompoundLinkPtr;

        CompoundLinkPtr = CompoundLinkPtr->next_compound_link;

        CompoundLinkPtr->compound_statement = NULL;
        CompoundLinkPtr->next_compound_link = NULL;
        CompoundLinkPtr->perv_compound_link = TempCompoundPtr;
	}

    /* Free/DeInit the last allocated link */
    if(CompoundLinkPtr->perv_compound_link != NULL)
    {
        TempCompoundPtr = CompoundLinkPtr->perv_compound_link;
        TempCompoundPtr->next_compound_link = NULL;
        free(CompoundLinkPtr);
    }

	/* Parse Compound Main */
	ProgramNode->compound_main = Parser_Compound_Statement_Main( Parser );

	return ProgramNode;
}
