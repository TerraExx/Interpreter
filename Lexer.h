/*
 * Lexer.h
 *
 *  Created on: 30.11.2017
 *      Author: posavi
 */

#ifndef LEXER_H_
#define LEXER_H_

#define KEY_WORD_NUM            8
#define KEY_WORD_MAX_LENGHT     32

typedef enum
{
	INTEGER_CONST,
	REAL_CONST,
	ID,
	VARIABLES,
	LCURLY,
	RCURLY,
	INT8,
	INT16,
	INT32,
	UINT8,
	UINT16,
	UINT32,
	FLOAT,
	ASSIGN,
	SEMICOL,
	PLUS,
	MINUS,
	NUM_OF_TOKEN_TYPES
} token_type;

typedef struct
{
	token_type	type;
	union
	{
		uint32_t	integer_const;
		float       float_const;
		char        string[32];
	} value;
} token;

typedef struct
{
    uint8_t string[KEY_WORD_MAX_LENGHT];
    token   keyword_token;
} keyword_struct;

keyword_struct  KeyWordArray[KEY_WORD_NUM];

typedef struct
{
	uint8_t*	text;
	size_t 		text_lenght;
	uint16_t	current_Pos;
	uint8_t*	current_Char;

	token (*fcnGetNextToken)(void*);
} lexer;

void Lexer_Init(lexer* Lexer, uint8_t* text);

token Lexer_GetNextToken(void* lexerPtr);

#endif /* LEXER_H_ */
