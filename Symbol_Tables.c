/*
 * Symbol_Tables.c
 *
 *  Created on: Dec 29, 2017
 *      Author: Josip
 */

#include "System.h"
#include "Symbol_Tables.h"

uint8_t SymbolCategoryString[SYMBOL_CATEGORY_NUM][MAX_SYMBOL_NAME_LENGHT] =
{
        "BUILT-IN TYPE",
        "VARIABLE",
        "COMPOUND",
        "PARAMETER"
};

s_symbol_symTable_link Symbol_Table_HeadLink;

s_symbol_symbol** Symbol_Table_CreateHash(uint16_t HashSize)
{
    s_symbol_symbol** HashPtr;

    /* Init */
    HashPtr = NULL;

    /* Allocate Hash Table */
    HashPtr = (s_symbol_symbol**) malloc(sizeof(s_symbol_symbol*) * HashSize);

    /* Init to NULL pointers */
    memset(HashPtr, 0, sizeof(s_symbol_symbol*) * HashSize);

    return HashPtr;
}

s_symbol_symbol_table* Symbol_Table_CreateTable(uint8_t* name, uint16_t hashSize)
{
    s_symbol_symbol_table* SymbolTable;

    SymbolTable = (s_symbol_symbol_table*) malloc(sizeof(s_symbol_symbol_table));

    /* Init */
    memset(SymbolTable->scope, 0, MAX_SYMBOL_NAME_LENGHT);
    strcpy( (char*)SymbolTable->scope, (const char*)name );
    SymbolTable->hash_info.hashSize = hashSize;

    /* Create Hash */
    SymbolTable->symbolHash = Symbol_Table_CreateHash( SymbolTable->hash_info.hashSize );

    /* Debug Printout */
    System_Print(" \n\tCreated Symbol Table: %s\n", name);

    return SymbolTable;
}

uint16_t Symbol_Table_Hash(uint8_t* key, uint16_t hashSize)
{
    uint16_t    Index;
    uint16_t    Sum;
    uint16_t    Lenght;
    uint8_t     Idx;


    /* Init */
    Index = 0;
    Sum = 0;
    Lenght = strlen((const char*)key);

    /* Hash Function */
    for( Idx = 0; Idx < Lenght; Idx++ )
    {
        Sum += *key;
        key++;
    }

    Index = Sum % hashSize;

    return Index;
}

void Symbol_Table_InsertSymbol( s_symbol_symbol* symbol, s_symbol_symbol_table* symbol_table )
{
    uint16_t    Index;
    uint16_t    Idx;

    s_symbol_symbol** TempSymbolPtr;

    enum e_symbol_table_insertSymbol
    {
        INSERTED,
        REDECLARED,
        HASH_FULL
    } Status;

    /* Init */
    TempSymbolPtr = NULL;

    /* Find hash index */
    Index = Symbol_Table_Hash(symbol->name, symbol_table->hash_info.hashSize);

    /* Try to insert the symbol into symbol table */
    TempSymbolPtr = ((s_symbol_symbol**)((uint32_t*)symbol_table->symbolHash + Index));
    if( *TempSymbolPtr == NULL )
    {
        *TempSymbolPtr = symbol;

        Status = INSERTED;
    }
    else if( strcmp((const char*)(*TempSymbolPtr)->name, (const char*)symbol->name) == 0 )
    {
        /* Found Variable already declared */
        Status = REDECLARED;
    }
    else
    {
        /* Find the first free slot or if the variable is already declared */
        for(Idx = Index + 1; Idx < Index + symbol_table->hash_info.hashSize; Idx++)
        {
            TempSymbolPtr = ((s_symbol_symbol**)((uint32_t*)symbol_table->symbolHash + (Idx % symbol_table->hash_info.hashSize)));

            if(*TempSymbolPtr == NULL)
            {
                /* Found Free Slot */
                *TempSymbolPtr = symbol;

                Status = INSERTED;
                break;
            }
            else if(strcmp((const char*)(*TempSymbolPtr)->name, (const char*)symbol->name) == 0)
            {
                /* Found Variable already declared */
                Status = REDECLARED;
            }
        }

        /* Check if there was no space */
        if(Idx == Index + symbol_table->hash_info.hashSize)
        {
            Status = HASH_FULL;
        }
    }

    /* Debug Printout */
    switch(Status)
    {
    case INSERTED:
        System_Print("\t\tINSERT: %s \t->\t %s\n", symbol->name, symbol_table->scope);
        break;
    case REDECLARED:
        System_Print("Semantic Error: Symbol %s redeclared in Symbol Table %s!\n", symbol->name, symbol_table->scope);
        exit(1);
        break;
    case HASH_FULL:
        System_Print("Semantic Error: Hash Table to small!\n");
        exit(1);
        break;
    }
}

s_symbol_symbol* Symbol_Table_GetSymbol( uint8_t* name, s_symbol_symbol_table* symbol_table )
{
    uint16_t    Index;
    uint16_t    Idx;

    s_symbol_symbol*    Symbol;

    enum e_symbol_table_insertSymbol
    {
        FOUND,
        NOT_FOUND
    } Status;

    /* Debug Printout */
    System_Print("\t\tLOOKUP: %s \t->\t %s\n", name, symbol_table->scope);

    /* Init */
    Symbol = NULL;

    /* Try and find the Variable in local/current scope */
    /* Hash the name */
    Index = Symbol_Table_Hash( name, symbol_table->hash_info.hashSize );

    /* Find if the symbol exists */
    for(Idx = Index; Idx < Index + symbol_table->hash_info.hashSize; Idx++)
    {
        Symbol = *((s_symbol_symbol**)((uint32_t*)symbol_table->symbolHash + (Idx % symbol_table->hash_info.hashSize)));
        if( Symbol != NULL )
        {
            /* Check if the symbol is correct */
            if( strcmp((const char*)name, (const char*)Symbol->name) == 0 )
            {
                /* Symbol found */
                Status = FOUND;
                break;
            }
        }
        else
        {
            /* Symbol Not Found */
            Status = NOT_FOUND;
            break;
        }
    }

    if(Idx == Index + symbol_table->hash_info.hashSize)
    {
        /* Symbol Not Found */
        Status = NOT_FOUND;
    }

    /* Try and find the Variable in global scope */
    if( Status == NOT_FOUND && symbol_table != Symbol_Table_HeadLink.symbolTable )
    {
        /* ReInit */
        symbol_table = Symbol_Table_HeadLink.symbolTable;
        Symbol = NULL;

        /* Call Get Symbol for global scope */
        Symbol = Symbol_Table_GetSymbol( name, symbol_table );
    }

    return Symbol;
}

s_symbol_symbol* Symbol_Table_CreateSymbol( e_symbol_category category, uint8_t* name, s_symbol_symbol* type )
{
    s_symbol_symbol* Symbol;

    Symbol = (s_symbol_symbol*) malloc(sizeof(s_symbol_symbol));

    /* Init */
    Symbol->category = category;

    memset(Symbol->name, 0, MAX_SYMBOL_NAME_LENGHT);
    strcpy( (char*)Symbol->name, (const char*)name );

    switch(category)
    {
    case SYMBOL_BUILTIN_TYPE:
        /* Fill built-in specific info */
        if(strcmp((const char*)name, (const char*)"int8") == 0)
        {
            Symbol->U.builtin.type = BIT_INT8;
        }
        else if(strcmp((const char*)name, (const char*)"int16") == 0)
        {
            Symbol->U.builtin.type = BIT_INT16;
        }
        else if(strcmp((const char*)name, (const char*)"int32") == 0)
        {
            Symbol->U.builtin.type = BIT_INT32;
        }
        else if(strcmp((const char*)name, (const char*)"uint8") == 0)
        {
            Symbol->U.builtin.type = BIT_UINT8;
        }
        else if(strcmp((const char*)name, (const char*)"uint16") == 0)
        {
            Symbol->U.builtin.type = BIT_UINT16;
        }
        else if(strcmp((const char*)name, (const char*)"uint32") == 0)
        {
            Symbol->U.builtin.type = BIT_UINT32;
        }
        else if(strcmp((const char*)name, (const char*)"float") == 0)
        {
            Symbol->U.builtin.type = BIT_FLOAT;
        }
        break;
    case SYMBOL_VARIABLE:
        /* Fill variable specific info */
        Symbol->U.variable.type = type;
        Symbol->U.variable.value = 0;
        break;
    case SYMBOL_COMPOUND:
        /* Fill compound specific info */
        Symbol->U.compound.type = type;
        break;
    case SYMBOL_PARAMETER:
        Symbol->U.parameter.type = type;
        break;
    case SYMBOL_CATEGORY_NUM:
        break;
    }

    return Symbol;
}

void Semantic_Analyzer_PrintoutSymbolTable(s_symbol_symbol_table* symbol_table)
{
    uint16_t            Idx;
    s_symbol_symbol*    TempSymbolPtr;

    /* Print Scope */
    System_Print(" \n \n");
    System_Print("\t\t\t\t\tSymbol Table: %s\n", symbol_table->scope);
    System_Print("\t------------------------------------------------------------------------------\n");
    System_Print("\t\tCategory\t|\tName\t|\tType\t|\tIndex\n");
    System_Print("\t------------------------------------------------------------------------------\n");


    /* Print Symbol Entries */
    for(Idx = 0; Idx < symbol_table->hash_info.hashSize; Idx++)
    {
        TempSymbolPtr = *((s_symbol_symbol**)((uint32_t*)symbol_table->symbolHash + Idx));
        if( TempSymbolPtr != NULL )
        {
            if( TempSymbolPtr->category == SYMBOL_BUILTIN_TYPE )
            {
                System_Print("\t\t%s\t|\t%s\t|\tn/a\t|\t%d\n", SymbolCategoryString[TempSymbolPtr->category], TempSymbolPtr->name, Idx);
            }
            else
            {
                System_Print("\t\t%s\t|\t%s\t|\t%s\t|\t%d\n"
                            , SymbolCategoryString[TempSymbolPtr->category]
                            , TempSymbolPtr->name
                            , ((s_symbol_symbol*)(TempSymbolPtr->U.variable.type))->name
                            , Idx
                            );
            }
            System_Print("\t------------------------------------------------------------------------------\n");
        }
    }
}

void Semantic_Analyzer_PrintoutSymbolTableAll()
{
    s_symbol_symTable_link* TempSymbolTablePtr;

    TempSymbolTablePtr = &Symbol_Table_HeadLink;

    while(TempSymbolTablePtr != NULL)
    {
        Semantic_Analyzer_PrintoutSymbolTable( TempSymbolTablePtr->symbolTable );

        TempSymbolTablePtr = TempSymbolTablePtr->next_symbol_table;
    }
}