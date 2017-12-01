/*
 ============================================================================
 Name        : Interpreter.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "Common.h"
#include "Lexer.h"

int main(void) {

	lexer Lexer;
	token Token;

	uint8_t	text[252] = "variables + { uint16 Idx; }";

	Lexer_Init(&Lexer, text);

	while(Lexer.current_Char != NULL)
	{
	    Token = Lexer.fcnGetNextToken(&Lexer);
	    if(Token.type == INTEGER_CONST)
	    {
	        printf("Token: %d\n", Token.value.integer_const);
	        fflush(stdout);
	    }
	    else
	    {
	        printf("Token: %s\n", Token.value.string);
	fflush(stdout);
	    }
	}


	return EXIT_SUCCESS;
}
