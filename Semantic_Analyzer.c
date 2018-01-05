/*
 * Semantic_Analyzer.c
 *
 *  Created on: Dec 29, 2017
 *      Author: Josip
 */

#include "System.h"
#include "Lexer.h"
#include "AST.h"
#include "Symbol_Tables.h"

struct s_semantic_analyzer_statistics
{
    s_symbol_symbol_table*   currentTable;
} Semantic_Analyzer_Statistics;

void Semantic_Analyzer_LineError( void* NodePtr )
{
    System_Print("Error on line %d:\n", ((s_ast_node_info*)NodePtr)->line);
}

void Semantic_Analyzer_Visit( void* NodePtr )
{
    uint8_t TempCount;

    void* TempNodePtr;

    s_symbol_symbol*    TempSymbolPtr;

    s_symbol_symTable_link* TempSymbolTableLinkPtr;
    s_symbol_symTable_link* TempPrevLinkPtr;

    /* Determine Node Type */
    switch( ((s_ast_node_info*)NodePtr)->type )
    {
    case PROGRAM:
        /* Global Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_program*)NodePtr)->global_var_decl_link;
        while( TempNodePtr != NULL && ((struct global_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Semantic_Analyzer_Visit( ((struct global_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct global_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Compound Statement Visit */
        TempNodePtr = (void*)&((s_ast_program*)NodePtr)->compound_link;
        while( TempNodePtr != NULL && ((struct compound_link*)TempNodePtr)->compound_statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct compound_link*)TempNodePtr)->compound_statement );

            TempNodePtr = (void*)((struct compound_link*)TempNodePtr)->next_compound_link;
        }

        /* Visit Compound Main */
        Semantic_Analyzer_Visit( ((s_ast_program*)NodePtr)->compound_main );

        break;

    case VAR_DECL:
        /* Lookup the variable type (in global scope) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_var_declaration*)NodePtr)->type->token.value.string, Symbol_Table_HeadLink.symbolTable);

        if(TempSymbolPtr == NULL)
        {
            /* Type is not declared as built-in type in global scope */
            Semantic_Analyzer_LineError(NodePtr);
            System_Print("Semantic Error: Type %s not found in global scope.\n", (uint8_t*)((s_ast_var_declaration*)NodePtr)->type->token.value.string);
            exit(1);
        }

        /* Variable Node Visits */
        TempNodePtr = (void*)&((s_ast_var_declaration*)NodePtr)->var_link;
        while( TempNodePtr != NULL && ((struct var_link*)TempNodePtr)->variable != NULL )
        {
            Symbol_Table_InsertSymbol
                    (
                        Symbol_Table_CreateSymbol( SYMBOL_VARIABLE, (uint8_t*)((struct var_link*)TempNodePtr)->variable->token.value.string, TempSymbolPtr )
                        , Semantic_Analyzer_Statistics.currentTable
                    );

            TempNodePtr = (void*)((struct var_link*)TempNodePtr)->next_var_link;
        }
        break;

    case COMPOUND_MAIN:
        /* Create and insert Compound Symbol into global scope */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)"uint8", Symbol_Table_HeadLink.symbolTable);

        Symbol_Table_InsertSymbol
                (
                    Symbol_Table_CreateSymbol( SYMBOL_COMPOUND, (uint8_t*)"main", TempSymbolPtr )
                    , Semantic_Analyzer_Statistics.currentTable
                );

        /* Create new Symbol Table */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*)"main", 128);

        /* Add the new Symbol Table to the linked list */
        TempSymbolTableLinkPtr = &Symbol_Table_HeadLink;

        while(TempSymbolTableLinkPtr->next_symbol_table != NULL)
        {
            TempSymbolTableLinkPtr = TempSymbolTableLinkPtr->next_symbol_table;
        }

        TempSymbolTableLinkPtr->next_symbol_table = (s_symbol_symTable_link*) malloc(sizeof(s_symbol_symTable_link));

        /* Init new link */
        TempPrevLinkPtr = TempSymbolTableLinkPtr;
        TempSymbolTableLinkPtr = TempSymbolTableLinkPtr->next_symbol_table;

        TempSymbolTableLinkPtr->next_symbol_table = NULL;
        TempSymbolTableLinkPtr->prev_symbol_table = TempPrevLinkPtr;

        TempSymbolTableLinkPtr->symbolTable = Semantic_Analyzer_Statistics.currentTable;

        /* Compound Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_compound_main*)NodePtr)->main_var_decl_link;
        while( TempNodePtr != NULL && ((struct main_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Semantic_Analyzer_Visit( ((struct main_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct main_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound_main*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }

        /* Return to global scope */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_HeadLink.symbolTable;
        break;

    case COMPOUND:
        /* Lookup the compound return type (in global scope) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_compound*)NodePtr)->return_type->type->token.value.string, Symbol_Table_HeadLink.symbolTable);

        if(TempSymbolPtr == NULL)
        {
            /* Type is not declared as built-in type in global scope */
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Return Type %s not found in global scope.\n", (uint8_t*)((s_ast_compound*)NodePtr)->return_type->type->token.value.string);
            exit(1);
        }

        /* Create and insert Compound Symbol into global scope */
        Symbol_Table_InsertSymbol
                (
                    Symbol_Table_CreateSymbol( SYMBOL_COMPOUND, (uint8_t*)((s_ast_compound*)NodePtr)->name.value.string, TempSymbolPtr )
                    , Semantic_Analyzer_Statistics.currentTable
                );

        /* Create new Symbol Table */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*)((s_ast_compound*)NodePtr)->name.value.string, 128);

        /* Add the new Symbol Table to the linked list */
        TempSymbolTableLinkPtr = &Symbol_Table_HeadLink;

        while(TempSymbolTableLinkPtr->next_symbol_table != NULL)
        {
            TempSymbolTableLinkPtr = TempSymbolTableLinkPtr->next_symbol_table;
        }

        TempSymbolTableLinkPtr->next_symbol_table = (s_symbol_symTable_link*) malloc(sizeof(s_symbol_symTable_link));

        /* Init new link */
        TempPrevLinkPtr = TempSymbolTableLinkPtr;
        TempSymbolTableLinkPtr = TempSymbolTableLinkPtr->next_symbol_table;

        TempSymbolTableLinkPtr->next_symbol_table = NULL;
        TempSymbolTableLinkPtr->prev_symbol_table = TempPrevLinkPtr;

        TempSymbolTableLinkPtr->symbolTable = Semantic_Analyzer_Statistics.currentTable;

        /* Parameter Visit */
        TempCount = 0;
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->parameter_link;
        while( TempNodePtr != NULL && ((struct parameter_link*)TempNodePtr)->parameter != NULL )
        {
            TempCount++;

            Semantic_Analyzer_Visit( ((struct parameter_link*)TempNodePtr)->parameter );

            TempNodePtr = (void*)((struct parameter_link*)TempNodePtr)->next_parameter_link;
        }

        /* Compound Variable Declaration Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->cmp_var_decl_link;
        while( TempNodePtr != NULL && ((struct cmp_var_decl_link*)TempNodePtr)->var_declaration != NULL )
        {
            Semantic_Analyzer_Visit( ((struct cmp_var_decl_link*)TempNodePtr)->var_declaration );

            TempNodePtr = (void*)((struct cmp_var_decl_link*)TempNodePtr)->next_var_decl_link;
        }

        /* Statement Node Visit */
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }

        /* Return to global scope */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_HeadLink.symbolTable;

        /* Add parameter number info to compound symbol table */
        TempSymbolPtr = Symbol_Table_GetSymbol((uint8_t*)((s_ast_compound*)NodePtr)->name.value.string, Symbol_Table_HeadLink.symbolTable);
        ((s_symbol_compoundStatementInfo*)TempSymbolPtr->info)->parameterNum = TempCount;
        ((s_symbol_compoundStatementInfo*)TempSymbolPtr->info)->compoundNode = NodePtr;

        break;

    case COMPOUND_CALL:
        /* Check if compound statement is declared in global scope */
        TempSymbolPtr = Symbol_Table_GetSymbol((uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string, Symbol_Table_HeadLink.symbolTable);

        if( TempSymbolPtr == NULL )
        {
            /* Var has not been declared*/
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Compound Statement %s not found in global scope.\n", (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string);
            exit(1);
        }

        /* Compound Call Argument Visit */
        TempCount = 0;
        TempNodePtr = (void*)&((s_ast_compoundStatementCall*)NodePtr)->argument_link;
        while( TempNodePtr != NULL && ((struct argument_link*)TempNodePtr)->argument != NULL )
        {
            TempCount++;

            Semantic_Analyzer_Visit( ((struct argument_link*)TempNodePtr)->argument );

            TempNodePtr = (void*)((struct argument_link*)TempNodePtr)->next_argument_link;
        }

        /* Check the number of arguments */
        if( TempCount < ((s_symbol_compoundStatementInfo*)TempSymbolPtr->info)->parameterNum )
        {
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Too few arguments for compound statement call %s.\n", (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string);
            exit(1);
        }
        else if( TempCount > ((s_symbol_compoundStatementInfo*)TempSymbolPtr->info)->parameterNum )
        {
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Too many arguments for compound statement call %s.\n", (uint8_t*)((s_ast_compoundStatementCall*)NodePtr)->cmpStatementName.value.string);
            exit(1);
        }
        break;

    case ARGUMENT:
        /* Visit Argument Nodes */
        Semantic_Analyzer_Visit( ((s_ast_compoundCallArg*)NodePtr)->argument );
        break;

    case COMPOUND_RETURN:
        /* No visit necessary */
        break;

    case RETURN_STATEMENT:
        /* Visit Return Value */
        Semantic_Analyzer_Visit( ((s_ast_compoundReturnStatement*)NodePtr)->value );
        break;

    case IF_STATEMENT:
        /* Visit If Conditions */
        TempNodePtr = (void*)&((s_ast_ifStatement*)NodePtr)->if_condition_link;
        while( TempNodePtr != NULL && ((struct if_condition_link*)TempNodePtr)->ifCondition != NULL )
        {
            Semantic_Analyzer_Visit( ((struct if_condition_link*)TempNodePtr)->ifCondition );

            TempNodePtr = (void*)((struct if_condition_link*)TempNodePtr)->next_ifCondition_link;
        }

        /* Visit Else Condition */
        if(((s_ast_ifStatement*)NodePtr)->elseCondition != NULL)
        {
            Semantic_Analyzer_Visit( ((s_ast_ifStatement*)NodePtr)->elseCondition );
        }
        break;

    case FOR_STATEMENT:
        /* Visit Init Statement */
        if( ((s_ast_forStatement*)NodePtr)->initStatement != NULL )
        {
            Semantic_Analyzer_Visit( ((s_ast_forStatement*)NodePtr)->initStatement );
        }

        /* Visit Post Statement */
        if( ((s_ast_forStatement*)NodePtr)->postStatement != NULL )
        {
            Semantic_Analyzer_Visit( ((s_ast_forStatement*)NodePtr)->postStatement );
        }

        /* Visit Condition Statement */
        if( ((s_ast_forStatement*)NodePtr)->condition != NULL )
        {
            Semantic_Analyzer_Visit( ((s_ast_forStatement*)NodePtr)->condition );
        }

        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_forStatement*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case BREAK_STATEMENT:
        /* No visit necessary */
        break;

    case IF_CONDITION:
        /* Visit Condition */
        Semantic_Analyzer_Visit( ((s_ast_ifCondition*)NodePtr)->condition );

        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_ifCondition*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case ELSE_CONDITION:
        /* Visit Statements */
        TempNodePtr = (void*)&((s_ast_elseCodnition*)NodePtr)->statement_link;
        while( TempNodePtr != NULL && ((struct s_ast_statement_link*)TempNodePtr)->statement != NULL )
        {
            Semantic_Analyzer_Visit( ((struct s_ast_statement_link*)TempNodePtr)->statement );

            TempNodePtr = (void*)((struct s_ast_statement_link*)TempNodePtr)->next_statement_link;
        }
        break;

    case PARAMETER:
        /* Lookup the variable type (in global scope) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_parameter*)NodePtr)->type->token.value.string, Symbol_Table_HeadLink.symbolTable);

        if(TempSymbolPtr == NULL)
        {
            /* Type is not declared as built-in type in global scope */
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Compound Parameter Type %s not found in global scope.\n", (uint8_t*)((s_ast_parameter*)NodePtr)->type->token.value.string);
            exit(1);
        }

        /* Create Parameter Entry in current symbol table */
        Symbol_Table_InsertSymbol
                (
                    Symbol_Table_CreateSymbol( SYMBOL_PARAMETER, (uint8_t*)((s_ast_parameter*)NodePtr)->variable->token.value.string, TempSymbolPtr )
                    , Semantic_Analyzer_Statistics.currentTable
                );
        break;

    case ASSIGNMENT:
        /* Variable (Left Operand) Visit */
        Semantic_Analyzer_Visit( ((s_ast_assignment*)NodePtr)->variable );

        /* Expression Visit */
        Semantic_Analyzer_Visit( ((s_ast_assignment*)NodePtr)->expression );
        break;

    case BINARY_OP:
        /* Visit Left Operand */
        Semantic_Analyzer_Visit( ((s_ast_binary*)NodePtr)->leftOperand );

        /* Visit Right Operand */
        Semantic_Analyzer_Visit( ((s_ast_binary*)NodePtr)->rightOperand );
        break;

    case UNARY_OP:
        /* Visit Operand */
        Semantic_Analyzer_Visit( ((s_ast_unary*)NodePtr)->operand );
        break;

    case NUM:
        /* No visit necessary */
        break;

    case TYPE:
        /* No visit necessary */
        break;

    case VAR:
        /* Check if variable is declared in global or current scope */
        TempSymbolPtr = Symbol_Table_GetSymbol((uint8_t*)((s_ast_variable*)NodePtr)->token.value.string, Semantic_Analyzer_Statistics.currentTable);

        if( TempSymbolPtr == NULL )
        {
            /* Var has not been declared*/
            Semantic_Analyzer_LineError( NodePtr );
            System_Print("Semantic Error: Variable %s not found in global scope or local scope.\n", (uint8_t*)((s_ast_variable*)NodePtr)->token.value.string);
            exit(1);
        }
        break;

    default:
        System_Print("Node Type Unknown.\n");
        exit(1);
        break;
    }
}

void Semantic_Analyzer_Walkthrought( s_ast_program* ProgramNode )
{
    /* Debug printout */
    System_Print(" \n \nCreating Symbol Tables:\n");

    /* Create Global System Table */
    Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*) "global", 128);

    /* Init System Table Head link */
    Symbol_Table_HeadLink.symbolTable = Semantic_Analyzer_Statistics.currentTable;
    Symbol_Table_HeadLink.next_symbol_table = NULL;
    Symbol_Table_HeadLink.prev_symbol_table = NULL;

    /* Populate Global System Table with Built-In Types */
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "int8", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "int16", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "int32", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "uint8", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "uint16", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "uint32", NULL), Semantic_Analyzer_Statistics.currentTable );
    Symbol_Table_InsertSymbol( Symbol_Table_CreateSymbol( SYMBOL_BUILTIN_TYPE, (uint8_t*) "float", NULL), Semantic_Analyzer_Statistics.currentTable );


    /* Visit all Program AST nodes */
    Semantic_Analyzer_Visit( ProgramNode );

    /* Debug printout */
    System_Print(" \n \nSymbol Tables Created:");

    Symbol_Table_PrintoutSymbolTableAll();
}
