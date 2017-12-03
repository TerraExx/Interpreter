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

	s_lexer_lexer Lexer;
	s_lexer_token Token;

	FILE*   FilePtr;
	size_t	FileSize;

	uint8_t*	Text;
	size_t	ReadSize;

	uint16_t	TokenCount = 0;

	FilePtr = fopen("C:\\Users\\Josip\\eclipse-workspace\\Interpreter\\src\\Interpreter\\Test_Script.txt", "r");

	if(FilePtr == NULL)
	{
		printf("File could not be opened.\n");
		exit(1);
	}

	fseek(FilePtr, 0, SEEK_END);
	FileSize = ftell(FilePtr);
	rewind(FilePtr);

	Text = (uint8_t*) malloc(FileSize);

	ReadSize = fread((void*)Text, 1, FileSize, FilePtr);
	Text[ReadSize] = 0;
	if(ReadSize == 0)
	{
		printf("File could not be read.\n");
		exit(1);
	}

	Lexer_Init(&Lexer, Text);

	while(Lexer.current_Char != NULL)
	{
		TokenCount++;

		Token = Lexer_GetNextToken(&Lexer);
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

	free((void*)Text);

	printf("Tken Count: %d\n",TokenCount);
	fflush(stdout);

	return EXIT_SUCCESS;
}
