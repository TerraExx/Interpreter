/*
 * Parser.h
 *
 *  Created on: Dec 2, 2017
 *      Author: Josip
 */

#ifndef INTERPRETER_PARSER_H_
#define INTERPRETER_PARSER_H_

typedef struct
{
	s_lexer_lexer*	lexer;
	s_lexer_token	current_token;
} s_parser_parser;

void Parser_Init(s_parser_parser* Parser, s_lexer_lexer* Lexer);

s_ast_program* Parser_Parse(s_parser_parser* Parser);

#endif /* INTERPRETER_PARSER_H_ */
