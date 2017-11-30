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

	uint8_t	text[20] = "612 + 8";

	Lexer_Init(&Lexer, text);
	Token = Lexer.fcnGetNextToken(&Lexer);

	return EXIT_SUCCESS;
}
