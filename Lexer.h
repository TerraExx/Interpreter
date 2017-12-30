/*
 * Lexer.h
 *
 *  Created on: 30.11.2017
 *      Author: posavi
 */

#ifndef LEXER_H_
#define LEXER_H_

#define KEY_WORD_NUM            9
#define KEY_WORD_MAX_LENGHT     32

#define VAR_NAME_MAX_LENGHT     32

typedef enum
{
	/* Key Words */
	VARIABLES,
	MAIN,
	/* Key Words - Built-in Types */
	INT8,
	INT16,
	INT32,
	UINT8,
	UINT16,
	UINT32,
	FLOAT,

	/* Other Tokens */
	INTEGER_CONST,
	REAL_CONST,
	ID,
	LPAREN,
	RPAREN,
	LCURLY,
	RCURLY,
	ASSIGN,
	COMMA,
	SEMICOL,
	PLUS,
	MINUS,
	MUL,
	DIV,
	TOKEN_TYPE_NUM
} e_lexer_token_type;

typedef struct
{
    uint16_t  line;
	e_lexer_token_type	type;
	union
	{
		uint32_t	integer_const;
		float       float_const;
		char        string[VAR_NAME_MAX_LENGHT];
	} value;
} s_lexer_token;

typedef struct
{
    uint8_t string[KEY_WORD_MAX_LENGHT];
    s_lexer_token   keyword_token;
} s_lexer_keyword;

s_lexer_keyword  KeyWordArray[KEY_WORD_NUM];

typedef struct
{
	uint8_t*	text;
	uint16_t	current_Pos;
	uint8_t*	current_Char;
} s_lexer_lexer;

void Lexer_Init(s_lexer_lexer* Lexer, uint8_t* text);

s_lexer_token Lexer_GetNextToken(s_lexer_lexer* Lexer);

#endif /* LEXER_H_ */
