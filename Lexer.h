/*
 * Lexer.h
 *
 *  Created on: 30.11.2017
 *      Author: posavi
 */

#ifndef LEXER_H_
#define LEXER_H_

#define KEY_WORD_MAX_LENGHT     32

#define VAR_NAME_MAX_LENGHT     32

typedef enum
{
	/* Key Words */
	VARIABLES,
	MAIN,
	RETURN,
	IF,
	ELSE,
	FOR,
	WHILE,
	BREAK,
	TW_TIMEOUT,
	/* Key Words - Built-in Types */
	INT8,
	INT16,
	INT32,
	UINT8,
	UINT16,
	UINT32,
	FLOAT,
	VOID,
	KEYWORD_NUM,

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
	LESS_THAN,
	LT_EQUAL,
	GREATER_THAN,
	GT_EQUAL,
	EQUAL,
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

typedef struct
{
	uint8_t*	text;
	uint16_t	current_Pos;
	uint8_t*	current_Char;
	uint16_t    line;
} s_lexer_lexer;

extern uint8_t TokenTypeString[TOKEN_TYPE_NUM][KEY_WORD_MAX_LENGHT];

void Lexer_Init(s_lexer_lexer* Lexer, uint8_t* text);

uint8_t Lexer_Peek( uint8_t* tokenText, s_lexer_lexer* Lexer );

s_lexer_token Lexer_GetNextToken(s_lexer_lexer* Lexer);

#endif /* LEXER_H_ */
