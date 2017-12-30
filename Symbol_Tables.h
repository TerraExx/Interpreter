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
    e_symbol_builtin_types  type;
} s_symbol_builtin;

typedef struct
{
    void*       type;  /* Pointer to Symbol of the Variable Type */
    uint32_t    value;
} s_symbol_variable;

typedef struct
{
    void*    type; /* Pointer to Symbol of the Compound Return Type */
} s_symbol_compound;

typedef struct
{
    void*    type; /* Pointer to Symbol of the Compound Parameter Type */
} s_symbol_parameter;

typedef struct
{
    e_symbol_category   category;
    uint8_t             name[MAX_SYMBOL_NAME_LENGHT];
    union
    {
        s_symbol_builtin    builtin;
        s_symbol_variable   variable;
        s_symbol_compound   compound;
        s_symbol_parameter  parameter;
    }U;

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

uint16_t Symbol_Table_Hash(uint8_t* key, uint16_t hashSize);

void Symbol_Table_InsertSymbol( s_symbol_symbol* symbol, s_symbol_symbol_table* symbol_table );

s_symbol_symbol* Symbol_Table_GetSymbol( uint8_t* name, s_symbol_symbol_table* symbol_table );

s_symbol_symbol* Symbol_Table_CreateSymbol( e_symbol_category category, uint8_t* name, s_symbol_symbol* type );

void Semantic_Analyzer_PrintoutSymbolTable(s_symbol_symbol_table* symbol_table);

void Semantic_Analyzer_PrintoutSymbolTableAll();

#endif /* INTERPRETER_SYMBOL_TABLES_H_ */
