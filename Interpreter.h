/*
 * Interpreter.h
 *
 *  Created on: Jan 1, 2018
 *      Author: Josip
 */

#ifndef INTERPRETER_INTERPRETER_H_
#define INTERPRETER_INTERPRETER_H_

typedef struct
{
    s_symbol_symbol_table*               symbolTable;
    struct s_interpreter_funcCallLink*   next_funcCall;
    struct s_interpreter_funcCallLink*   prev_funcCall;
} s_interpreter_funcCallLink;

void Interpreter_Walkthrought( s_ast_program* ProgramNode );

#endif /* INTERPRETER_INTERPRETER_H_ */
