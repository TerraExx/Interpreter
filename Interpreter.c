/*
 * Interpreter.c
 *
 *  Created on: Jan 1, 2018
 *      Author: Josip
 */

#include "System.h"
#include "Lexer.h"
#include "AST.h"
#include "Symbol_Tables.h"
#include "Interpreter.h"

s_interpreter_funcCallLink*  Interpreter_FuncCallStack;

void Interpreter_PushFuncCall( s_symbol_symbol_table* symbolTable )
{
    s_interpreter_funcCallLink* TempLinkPtr;

    /* Debug Printout */
    System_Print("\tFunction Call Stack: Push %s\n", symbolTable->scope);

    /* Init */
    TempLinkPtr = Interpreter_FuncCallStack;

    /* GoTo last link */
    if( Interpreter_FuncCallStack != NULL )
    {
        while( Interpreter_FuncCallStack != NULL )
        {
            Interpreter_FuncCallStack = (s_interpreter_funcCallLink*)Interpreter_FuncCallStack->next_funcCall;
        }
    }

    /* Allocate & Init new link */
    Interpreter_FuncCallStack = (s_interpreter_funcCallLink*) malloc(sizeof(s_interpreter_funcCallLink));

    Interpreter_FuncCallStack->symbolTable = NULL;
    Interpreter_FuncCallStack->next_funcCall = NULL;
    Interpreter_FuncCallStack->prev_funcCall = NULL;

    /* Push symbolTable to stack */
    Interpreter_FuncCallStack->symbolTable = symbolTable;
    Interpreter_FuncCallStack->prev_funcCall = (struct s_interpreter_funcCallLink*)TempLinkPtr;

    /* Alloc memory for all variables in symbolTable */
    Symbol_Table_MemAllocVar( symbolTable );
}

void Interpreter_PopFuncCall()
{
    /* Debug Printout */
    System_Print("\tFunction Call Stack: Pop %s\n", Interpreter_FuncCallStack->symbolTable->scope);

    /* Free memory of all variables in the current symbol table */
    Symbol_Table_MemFreeVar( Interpreter_FuncCallStack->symbolTable );

    /* Free the last Function Call Stack Link */
    if(Interpreter_FuncCallStack->prev_funcCall != NULL)
    {
        Interpreter_FuncCallStack = (s_interpreter_funcCallLink*)Interpreter_FuncCallStack->prev_funcCall;
        free(Interpreter_FuncCallStack->next_funcCall);
    }
    else
    {
        free(Interpreter_FuncCallStack);
    }
}

uint32_t Interpreter_Visit( void* NodePtr )
{
    uint32_t Return;

    void* TempNodePtr;
    void* TempNodePtr_2;

    s_ast_compound*  TempCompundNode;

    s_symbol_symbol* TempSymbolPtr;

    /* Init */
    Return = 0;

    /* Determine Node Type */
    switch( ((s_ast_node_info*)NodePtr)->type )
    {
    case PROGRAM:
        /* Init Function Call Stack with global scope */
        Interpreter_PushFuncCall( Symbol_Table_HeadLink.symbolTable );

        /* Visit Compound Main */
        Interpreter_Visit( ((s_ast_program*)NodePtr)->compound_main );

        /* Debug Printout */
        Symbol_Table_PrintoutSymbolTable( Symbol_Table_HeadLink.symbolTable );

        /* Pop global scope from Function Call Stack */
        Interpreter_PopFuncCall();
        break;

    case VAR_DECL:
        break;

    case COMPOUND_MAIN:
        /* Push main scope to Function Call Stack */
        Interpreter_PushFuncCall( Symbol_Table_GetTable( (uint8_t*) "main") );

        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound_main*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct statement_link*)TempNodePtr)->statement != NULL )
        {
            Interpreter_Visit( ((struct statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct statement_link*)TempNodePtr)->next_statement_link;
        }

        /* Debug Printout */
        Symbol_Table_PrintoutSymbolTable( Symbol_Table_HeadLink.symbolTable );
        Symbol_Table_PrintoutSymbolTable( Interpreter_FuncCallStack->symbolTable );

        /* Pop main scope from Function Call Stack */
        Interpreter_PopFuncCall();
        break;

    case COMPOUND:
        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct c_statement_link*)TempNodePtr)->statement != NULL )
        {
            Interpreter_Visit( ((struct c_statement_link*)TempNodePtr)->statement );

            if( ((s_ast_node_info*)((struct c_statement_link*)TempNodePtr)->statement)->type == RETURN_STATEMENT  )
            {
                /* Return Statement was executed. Stop function execution and return. */
                break;
            }

            TempNodePtr = (void*)((struct c_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case COMPOUND_CALL:
        /* Push compound statement scope to Function Call Stack */
        Interpreter_PushFuncCall( Symbol_Table_GetTable( (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string ) );

        /* Get compound node pointer */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string, Symbol_Table_HeadLink.symbolTable );
        TempCompundNode = ((s_symbol_compoundStatementInfo*)TempSymbolPtr->info)->compoundNode;

        /* Init Compound Parameters */
        /* Compound Call Argument Visit */
        TempNodePtr   = (void*)&((s_ast_compoundStatementCall*)NodePtr)->argument_link;
        TempNodePtr_2 = (void*)&((s_ast_compound*)TempCompundNode)->parameter_link;
        while( TempNodePtr != NULL && ((struct argument_link*)TempNodePtr)->argument != NULL )
        {
            /* Get Parameter Symbol */
            TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((struct parameter_link*)TempNodePtr_2)->parameter->variable->token.value.string, Interpreter_FuncCallStack->symbolTable );

            *(uint32_t*)TempSymbolPtr->value = Interpreter_Visit( ((struct argument_link*)TempNodePtr)->argument->argument );

            TempNodePtr = (void*)((struct argument_link*)TempNodePtr)->next_argument_link;
            TempNodePtr_2 = (void*)((struct parameter_link*)TempNodePtr_2)->next_parameter_link;
        }

        /* Get Compound Symbol */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string, Symbol_Table_HeadLink.symbolTable );

        /* Visit Compound Node */
        Interpreter_Visit( TempCompundNode );

        /* Set Compound Value to Return */
        Return = *(uint32_t*)TempSymbolPtr->value;

        /* Debug Printout */
        Symbol_Table_PrintoutSymbolTable( Symbol_Table_HeadLink.symbolTable );
        Symbol_Table_PrintoutSymbolTable( Interpreter_FuncCallStack->symbolTable );

        /* Pop compound scope from Function Call Stack */
        Interpreter_PopFuncCall();
        break;

    case ARGUMENT:
        break;

    case COMPOUND_RETURN:
        break;

    case RETURN_STATEMENT:
        /* Get compound symbol */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)Interpreter_FuncCallStack->symbolTable->scope, Symbol_Table_HeadLink.symbolTable );

        /* Set compound value */
        *(uint32_t*)TempSymbolPtr->value = Interpreter_Visit( ((s_ast_compoundReturnStatement*)NodePtr)->value );

        /* Set Return Value */
        Return = FALSE;
        break;

    case IF_STATEMENT:
        /* Visit If Conditions */
        TempNodePtr = (void*)&((s_ast_ifStatement*)NodePtr)->if_condition_link;
        while( TempNodePtr != NULL && ((struct if_condition_link*)TempNodePtr)->ifCondition != NULL )
        {
            Return = Interpreter_Visit( ((struct if_condition_link*)TempNodePtr)->ifCondition );

            /* Check if the condition was executed */
            if( Return == TRUE )
            {
                /* Condition was executed, skip the rest of if statement */
                break;
            }
            else
            {
                TempNodePtr = (void*)((struct if_condition_link*)TempNodePtr)->next_ifCondition_link;
            }
        }

        if( Return == FALSE )
        {
            /* Visit Else Condition */
            if(((s_ast_ifStatement*)NodePtr)->elseCondition != NULL)
            {
                Interpreter_Visit( ((s_ast_ifStatement*)NodePtr)->elseCondition );
            }
        }
        break;

    case IF_CONDITION:
        /* Visit Condition */
        if( Interpreter_Visit( ((s_ast_ifCondition*)NodePtr)->condition ) )
        {
            /* Visit Statements */
            TempNodePtr = (void*)&((s_ast_ifCondition*)NodePtr)->if_statement_link;
            while( TempNodePtr != NULL && ((struct if_statement_link*)TempNodePtr)->statement != NULL )
            {
                Interpreter_Visit( ((struct if_statement_link*)TempNodePtr)->statement );

                TempNodePtr = (void*)((struct if_statement_link*)TempNodePtr)->next_statement_link;
            }

            /* Set Return */
            Return = TRUE;
        }
        break;

    case ELSE_CONDITION:
        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_elseCodnition*)NodePtr)->else_statement_link;
        while( TempNodePtr != NULL && ((struct else_statement_link*)TempNodePtr)->statement != NULL )
        {
            Interpreter_Visit( ((struct else_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct else_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case PARAMETER:
        break;

    case ASSIGNMENT:
        /* Get left operand (variable) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_assignment*)NodePtr)->variable->token.value.string, Interpreter_FuncCallStack->symbolTable );

        /* Assign right operand (expression) */
        *(uint32_t*)(TempSymbolPtr->value) = Interpreter_Visit( ((s_ast_assignment*)NodePtr)->expression );
        break;

    case BINARY_OP:
        /* Do Binary Operation */
        switch(((s_ast_binary*)NodePtr)->token.type)
        {
        case PLUS:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) + Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case MINUS:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) - Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case MUL:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) * Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case LESS_THAN:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) < Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case LT_EQUAL:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) <= Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case GREATER_THAN:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) > Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case GT_EQUAL:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) >= Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        case EQUAL:
            Return = Interpreter_Visit(((s_ast_binary*)NodePtr)->leftOperand) == Interpreter_Visit(((s_ast_binary*)NodePtr)->rightOperand);
            break;
        default:
            break;
        }

        break;

    case UNARY_OP:
        break;

    case NUM:
        /* Return Constant Value */
        Return = ((s_ast_num*)NodePtr)->token.value.integer_const;
        break;

    case TYPE:
        break;

    case VAR:
        /* Return Variable Value */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_variable*)NodePtr)->token.value.string, Interpreter_FuncCallStack->symbolTable );
        Return = *(uint32_t*)TempSymbolPtr->value;
        break;

    default:
        printf("Node Type Unknown: %d\n", ((s_ast_node_info*)NodePtr)->type);
        exit(1);
        break;
    }

    return Return;
}

void Interpreter_Walkthrought( s_ast_program* ProgramNode )
{
    /* Debug Printout */
    System_Print(" \nInterpretation starting:\n");

    /* Visit AST nodes */
    Interpreter_Visit( ProgramNode );
}
