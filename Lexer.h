/*
 * Lexer.h
 *
 *  Created on: 30.11.2017
 *      Author: posavi
 */

#ifndef LEXER_H_
#define LEXER_H_

typedef enum
{
	INTEGER_CONST,
	REAL_CONST,
	ASSIGN,
	PLUS,
	MINUS,
	NUM_OF_TOKEN_TYPES
} token_type;

typedef struct
{
	token_type	type;
	union
	{
		uint32_t	integer;
	} value;
} token;

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
