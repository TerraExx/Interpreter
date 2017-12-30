/*
 ============================================================================
 Name        : Interpreter.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include "System.h"
#include "Lexer.h"
#include "AST.h"
#include "Debug_Walker.h"
#include "Parser.h"
#include "Semantic_Analyzer.h"

int main(void) {

	s_lexer_lexer   Lexer;
	s_parser_parser Parser;
	s_ast_program*  ProgramNode;


	FILE*   FilePtr;
	size_t	FileSize;

	uint8_t*	Text;
	size_t	ReadSize;

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
	Parser_Init(&Parser, &Lexer);

	ProgramNode = Parser_Parse(&Parser);

	Debug_Walker_Walkthrought(ProgramNode);

	Semantic_Analyzer_Walkthrought( ProgramNode );

	free((void*)Text);

	return EXIT_SUCCESS;
}
