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

void Semantic_Analyzer_Visit( void* NodePtr )
{
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
        /* Create new Symbol Table */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*)"main", 512);

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

        /* Return to global scope */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_HeadLink.symbolTable;
        break;

    case COMPOUND:
        /* Lookup the compound return type (in global scope) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_compound*)NodePtr)->return_type->type->token.value.string, Symbol_Table_HeadLink.symbolTable);

        if(TempSymbolPtr == NULL)
        {
            /* Type is not declared as built-in type in global scope */
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
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*)((s_ast_compound*)NodePtr)->name.value.string, 512);

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
        TempNodePtr = (void*)&((s_ast_compound*)NodePtr)->parameter_link;
        while( TempNodePtr != NULL && ((struct parameter_link*)TempNodePtr)->parameter != NULL )
        {
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

        /* Return to global scope */
        Semantic_Analyzer_Statistics.currentTable = Symbol_Table_HeadLink.symbolTable;

        break;

    case COMPOUND_RETURN:
        break;

    case PARAMETER:
        /* Lookup the variable type (in global scope) */
        TempSymbolPtr = Symbol_Table_GetSymbol( (uint8_t*)((s_ast_parameter*)NodePtr)->type->token.value.string, Symbol_Table_HeadLink.symbolTable);

        if(TempSymbolPtr == NULL)
        {
            /* Type is not declared as built-in type in global scope */
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
        break;

    case BINARY_OP:
        break;

    case UNARY_OP:
        break;

    case NUM:
        break;

    case TYPE:
        break;

    case VAR:
        break;

    default:
        printf("Node Type Unknown: %d\n", ((s_ast_node_info*)NodePtr)->type);
        exit(1);
        break;
    }
}

void Semantic_Analyzer_Walkthrought( s_ast_program* ProgramNode )
{
    /* Debug printout */
    System_Print(" \n \nCreating Symbol Tables:\n \n");

    /* Create Global System Table */
    Semantic_Analyzer_Statistics.currentTable = Symbol_Table_CreateTable( (uint8_t*) "global", 1024);

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

    Semantic_Analyzer_PrintoutSymbolTableAll();
}
