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
void* Parser_Statement(s_parser_parser* Parser);
s_ast_compoundStatementCall* Parser_Compound_Call(s_parser_parser* Parser);

/* Function Implementation */

void Parser_Init(s_parser_parser* Parser, s_lexer_lexer* Lexer)
{
	Parser->lexer = Lexer;
	Parser->current_token = Lexer_GetNextToken(Lexer);
}

void Parser_LineError(s_parser_parser* Parser)
{
    System_Print("Error on line %d:\n", Parser->lexer->line);
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
	    Parser_LineError( Parser );
	    System_Print("Parser Error: Wrong Token Type -> Current_Token: %s, Expected: %s.\n", TokenTypeString[Parser->current_token.type], TokenTypeString[Type]);
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
  Type_SpecNode->info.line = Parser->lexer->line;

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
      Parser_LineError( Parser );
      System_Print("Unknown type: %s\n", Parser->current_token.value.string);
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
  VariableNode->info.line = Parser->lexer->line;

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

        if( Lexer_Peek( (uint8_t*)"(", Parser->lexer ) == TRUE )
        {
            /* Compound Statement Call */
            Factor = Parser_Compound_Call( Parser );
        }
        else
        {
            /* Init */
            VariableNode->info.type = VAR;

            VariableNode->token = Parser->current_token;

            Parser_Eat( Parser, ID );

            Factor = VariableNode;
        }

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

    while(Parser->current_token.type == PLUS          ||
          Parser->current_token.type == MINUS         ||
          Parser->current_token.type == LESS_THAN     ||
          Parser->current_token.type == LT_EQUAL      ||
          Parser->current_token.type == GREATER_THAN  ||
          Parser->current_token.type == GT_EQUAL      ||
          Parser->current_token.type == EQUAL )
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
        else if(Parser->current_token.type == LESS_THAN)
        {
            Parser_Eat( Parser, LESS_THAN );
        }
        else if(Parser->current_token.type == LT_EQUAL)
        {
            Parser_Eat( Parser, LT_EQUAL );
        }
        else if(Parser->current_token.type == GREATER_THAN)
        {
            Parser_Eat( Parser, GREATER_THAN );
        }
        else if(Parser->current_token.type == GT_EQUAL)
        {
            Parser_Eat( Parser, GT_EQUAL );
        }
        else if(Parser->current_token.type == EQUAL)
        {
            Parser_Eat( Parser, EQUAL );
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
  DeclarationNode->info.line = Parser->lexer->line;

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

void Parser_Statement_list(s_parser_parser* Parser, s_ast_statement_link* StatementLinkPtr)
{
    struct statement_link*       TempPtr;

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
}

s_ast_compoundReturnStatement* Parser_Compound_Return_Statement(s_parser_parser* Parser)
{
    s_ast_compoundReturnStatement* ReturnStatementNode;

    ReturnStatementNode = (s_ast_compoundReturnStatement*)malloc(sizeof(s_ast_compoundReturnStatement));

    /* Init */
    ReturnStatementNode->info.type = RETURN_STATEMENT;
    ReturnStatementNode->info.line = Parser->lexer->line;

    ReturnStatementNode->value = NULL;

    /* Eat Return token */
    Parser_Eat( Parser, RETURN );

    /* Parse Factor */
    ReturnStatementNode->value = Parser_Factor(Parser);

    return ReturnStatementNode;
}

s_ast_compoundCallArg* Parser_Compound_Call_Argument(s_parser_parser* Parser)
{
    s_ast_compoundCallArg* ArgumentNode;

    ArgumentNode = (s_ast_compoundCallArg*) malloc(sizeof(s_ast_compoundCallArg));

    /* Init */
    ArgumentNode->info.type = ARGUMENT;
    ArgumentNode->info.line = Parser->lexer->line;

    if( Parser->current_token.type == ID )
    {
        /* Parse Variable */
        ArgumentNode->argument = Parser_Variable( Parser );
    }
    else if( Parser->current_token.type == INTEGER_CONST )
    {
        /* Parse Variable */
        ArgumentNode->argument = Parser_Factor( Parser );
    }
    else if( Parser->current_token.type == REAL_CONST )
    {
        /* Parse Variable */
        ArgumentNode->argument = Parser_Factor( Parser );
    }

    return ArgumentNode;
}

s_ast_compoundStatementCall* Parser_Compound_Call(s_parser_parser* Parser)
{
    s_ast_compoundStatementCall*  CompoundCallNode;

    struct argument_link*   ArgLinkPtr;
    struct argument_link*   TempArgPtr;

    CompoundCallNode = (s_ast_compoundStatementCall*)malloc(sizeof(s_ast_compoundStatementCall));

    /* Init */
    CompoundCallNode->info.type = COMPOUND_CALL;
    CompoundCallNode->info.line = Parser->lexer->line;

    CompoundCallNode->argument_link.argument = NULL;
    CompoundCallNode->argument_link.next_argument_link = NULL;
    CompoundCallNode->argument_link.perv_argument_link = NULL;

    /* Parse variable (left operand) */
    CompoundCallNode->cmpStatementName = Parser->current_token;
    Parser_Eat( Parser, ID );

    /* Parse Arguments */
    Parser_Eat( Parser, LPAREN );

    ArgLinkPtr = &CompoundCallNode->argument_link;

    for(; Parser->current_token.type != RPAREN ; Parser_Eat(Parser, COMMA))
    {
        ArgLinkPtr->argument = Parser_Compound_Call_Argument( Parser );

        ArgLinkPtr->next_argument_link = (struct argument_link*) malloc(sizeof(struct argument_link));

        /* Init new link */
        TempArgPtr = ArgLinkPtr;

        ArgLinkPtr = ArgLinkPtr->next_argument_link;

        ArgLinkPtr->argument = NULL;
        ArgLinkPtr->next_argument_link = NULL;
        ArgLinkPtr->perv_argument_link = TempArgPtr;

        if(Parser->current_token.type == RPAREN)
        {
            break;
        }
    }

    /* Free/DeInit the last allocated link */
    if(ArgLinkPtr->perv_argument_link != NULL)
    {
        TempArgPtr = ArgLinkPtr->perv_argument_link;
        TempArgPtr->next_argument_link = NULL;
        free(ArgLinkPtr);
    }

    Parser_Eat( Parser, RPAREN );

    return CompoundCallNode;
}

s_ast_assignment* Parser_Assignment_Statement(s_parser_parser* Parser)
{
    s_ast_assignment* AssignmentStatementNode;

    AssignmentStatementNode = (s_ast_assignment*)malloc(sizeof(s_ast_assignment));

    /* Init */
    AssignmentStatementNode->info.type = ASSIGNMENT;
    AssignmentStatementNode->info.line = Parser->lexer->line;

    AssignmentStatementNode->variable = NULL;

    /* Parse variable (left operand) */
    AssignmentStatementNode->variable = Parser_Variable( Parser );

    Parser_Eat( Parser, ASSIGN );

    /* Parse right side */
    if( Parser->current_token.type == ID && Lexer_Peek( (uint8_t*)"(", Parser->lexer ) == TRUE )
    {
        /* Compound Statement Call */
        AssignmentStatementNode->expression = Parser_Compound_Call( Parser );
    }
    else
    {
        /* Expression */
        AssignmentStatementNode->expression = Parser_Expression( Parser );
    }

    return AssignmentStatementNode;
}

s_ast_elseCodnition* Parser_Else_Condition(s_parser_parser* Parser)
{
    s_ast_elseCodnition* ElseConditionNode;

    struct else_statement_link*       StatementLinkPtr;
    struct else_statement_link*       TempPtr;

    ElseConditionNode = (s_ast_elseCodnition*)malloc(sizeof(s_ast_elseCodnition));

    /* Init */
    ElseConditionNode->info.type = ELSE_CONDITION;
    ElseConditionNode->info.line = Parser->lexer->line;

    ElseConditionNode->else_statement_link.statement = NULL;
    ElseConditionNode->else_statement_link.next_statement_link = NULL;
    ElseConditionNode->else_statement_link.perv_statement_link = NULL;

    /* Parser Else Statement Block */
    Parser_Eat( Parser, LCURLY);

    StatementLinkPtr = &ElseConditionNode->else_statement_link;

    /* Parse Statements */
    while(Parser->current_token.type != RCURLY)
    {
        StatementLinkPtr->statement = Parser_Statement( Parser );

        StatementLinkPtr->next_statement_link = (struct else_statement_link*) malloc(sizeof(struct else_statement_link));

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

    Parser_Eat( Parser, RCURLY);


    return ElseConditionNode;
}

s_ast_ifCondition* Parser_If_Condition(s_parser_parser* Parser)
{
    s_ast_ifCondition* IfConditionNode;

    struct if_statement_link*       StatementLinkPtr;
    struct if_statement_link*       TempPtr;

    IfConditionNode = (s_ast_ifCondition*)malloc(sizeof(s_ast_ifCondition));

    /* Init */
    IfConditionNode->info.type = IF_CONDITION;
    IfConditionNode->info.line = Parser->lexer->line;

    IfConditionNode->condition = NULL;

    IfConditionNode->if_statement_link.statement = NULL;
    IfConditionNode->if_statement_link.next_statement_link = NULL;
    IfConditionNode->if_statement_link.perv_statement_link = NULL;

    /* Parser If Statement Block */
    Parser_Eat( Parser, IF );

    Parser_Eat( Parser, LPAREN );

    IfConditionNode->condition = Parser_Expression( Parser );

    Parser_Eat( Parser, RPAREN );
    Parser_Eat( Parser, LCURLY);

    StatementLinkPtr = &IfConditionNode->if_statement_link;

    /* Parse Statements */
    while(Parser->current_token.type != RCURLY)
    {
        StatementLinkPtr->statement = Parser_Statement( Parser );

        StatementLinkPtr->next_statement_link = (struct if_statement_link*) malloc(sizeof(struct if_statement_link));

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

    Parser_Eat( Parser, RCURLY);

    return IfConditionNode;
}

s_ast_ifStatement* Parser_If_Statement(s_parser_parser* Parser)
{
    s_ast_ifStatement* IfStatementNode;

    struct if_condition_link*   IfCondLinkPtr;
    struct if_condition_link*   TempIfCondPtr;

    IfStatementNode = (s_ast_ifStatement*)malloc(sizeof(s_ast_ifStatement));

    /* Init */
    IfStatementNode->info.type = IF_STATEMENT;
    IfStatementNode->info.line = Parser->lexer->line;

    IfStatementNode->if_condition_link.ifCondition = NULL;
    IfStatementNode->if_condition_link.next_ifCondition_link = NULL;
    IfStatementNode->if_condition_link.perv_ifCondition_link = NULL;

    IfStatementNode->elseCondition = NULL;

    IfCondLinkPtr = NULL;
    TempIfCondPtr = NULL;

    /* Parse If Statement */
    IfStatementNode->if_condition_link.ifCondition = Parser_If_Condition( Parser );

    IfCondLinkPtr = &IfStatementNode->if_condition_link;
    TempIfCondPtr = IfCondLinkPtr;

    /* Parse Else and Else If Statements */
    while( Parser->current_token.type == ELSE )
    {
        Parser_Eat( Parser, ELSE );

        if( Parser->current_token.type == IF )
        {
            IfCondLinkPtr->next_ifCondition_link = (struct if_condition_link*)malloc(sizeof(struct if_condition_link));
            IfCondLinkPtr = IfCondLinkPtr->next_ifCondition_link;

            /* Init new link */
            IfCondLinkPtr->ifCondition = NULL;
            IfCondLinkPtr->next_ifCondition_link = NULL;
            IfCondLinkPtr->perv_ifCondition_link = TempIfCondPtr;

            /* Parser an else if statement */
            IfCondLinkPtr->ifCondition = Parser_If_Condition( Parser );

            TempIfCondPtr = IfCondLinkPtr;
        }
        else if( Parser->current_token.type == LCURLY )
        {
            /* Parser an else statement */
            IfStatementNode->elseCondition = Parser_Else_Condition( Parser );
        }
    }

    return IfStatementNode;
}

void* Parser_Statement(s_parser_parser* Parser)
{
    void*   Statement;

    /* Init */
    Statement = NULL;

    /* Check statement type */
    if( Parser->current_token.type == ID )
    {
        if( Lexer_Peek( (uint8_t*)"=", Parser->lexer ) == TRUE )
        {
            /* Assignment Statement */
            Statement = Parser_Assignment_Statement( Parser );
        }
        else if( Lexer_Peek( (uint8_t*)"(", Parser->lexer ) == TRUE )
        {
            /* Compound Statement Call */
            Statement = Parser_Compound_Call( Parser );
        }

        Parser_Eat( Parser, SEMICOL );
    }
    else if( Parser->current_token.type == RETURN )
    {
        /* Return Statement */
        Statement = Parser_Compound_Return_Statement( Parser );

        Parser_Eat( Parser, SEMICOL );
    }
    else if( Parser->current_token.type == IF )
    {
        /* if Statement */
        Statement = Parser_If_Statement( Parser );
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
	CompoundMainNode->info.line = Parser->lexer->line;

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
    CompoundParameterNode->info.line = Parser->lexer->line;

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
    CompoundReturnNode->info.line = Parser->lexer->line;

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
    CoumpoundStatementNode->info.line = Parser->lexer->line;

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
	ProgramNode->info.line = 0;

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
