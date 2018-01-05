/*
 * Symbol_Tables.h
 *
 *  Created on: Dec 29, 2017
 *      Author: Josip
 */

#ifndef INTERPRETER_SYMBOL_TABLES_H_
#define INTERPRETER_SYMBOL_TABLES_H_

#define MAX_SYMBOL_NAME_LENGHT  32

typedef enum
{
    SYMBOL_BUILTIN_TYPE,
    SYMBOL_VARIABLE,
    SYMBOL_COMPOUND,
    SYMBOL_PARAMETER,
    SYMBOL_CATEGORY_NUM
} e_symbol_category;

typedef enum
{
    BIT_INT8,
    BIT_INT16,
    BIT_INT32,
    BIT_UINT8,
    BIT_UINT16,
    BIT_UINT32,
    BIT_FLOAT,
    BIT_NUM
} e_symbol_builtin_types;

typedef struct
{
    uint8_t parameterNum;

    void*   compoundNode;
    void*   returnNode;

    uint8_t returnExecuted;
    uint8_t breakExecuted;
} s_symbol_compoundStatementInfo;

typedef struct
{
    e_symbol_category   category;
    uint8_t             name[MAX_SYMBOL_NAME_LENGHT];
    void*               type;
    void*               value;
    void*               info;
} s_symbol_symbol;

typedef struct
{
    uint8_t scope[32];

    struct hash_info
    {
        uint16_t    hashSize;
    } hash_info;

    s_symbol_symbol**    symbolHash;
} s_symbol_symbol_table;

typedef struct s_symbol_symTable_link
{
    s_symbol_symbol_table*           symbolTable;
    struct s_symbol_symTable_link*   next_symbol_table;
    struct s_symbol_symTable_link*   prev_symbol_table;
} s_symbol_symTable_link;

extern s_symbol_symTable_link Symbol_Table_HeadLink;

s_symbol_symbol** Symbol_Table_CreateHash(uint16_t HashSize);

s_symbol_symbol_table* Symbol_Table_CreateTable(uint8_t* name, uint16_t hashSize);

s_symbol_symbol_table* Symbol_Table_GetTable( uint8_t* name );

uint16_t Symbol_Table_Hash(uint8_t* key, uint16_t hashSize);

void Symbol_Table_InsertSymbol( s_symbol_symbol* symbol, s_symbol_symbol_table* symbol_table );

s_symbol_symbol* Symbol_Table_GetSymbol( uint8_t* name, s_symbol_symbol_table* symbol_table );

s_symbol_symbol* Symbol_Table_CreateSymbol( e_symbol_category category, uint8_t* name, s_symbol_symbol* type );

void Symbol_Table_MemAllocVar(s_symbol_symbol_table* symbol_table);

void Symbol_Table_MemFreeVar(s_symbol_symbol_table* symbol_table);

void Symbol_Table_PrintoutSymbolTable(s_symbol_symbol_table* symbol_table);

void Symbol_Table_PrintoutSymbolTableAll();

#endif /* INTERPRETER_SYMBOL_TABLES_H_ */
