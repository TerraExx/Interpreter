#include "Common.h"
#include "Lexer.h"

#include <ctype.h>

void Lexer_Init(lexer* Lexer, uint8_t* text)
{
	Lexer->text = text;
	Lexer->text_lenght = strlen((const char*)text);

	Lexer->current_Pos = 0;
	Lexer->current_Char = text;

	Lexer->fcnGetNextToken = Lexer_GetNextToken;
}

void Lexer_Advance(void* lexerPtr)
{
    lexer* Lexer;
    Lexer = (lexer*)lexerPtr;

    Lexer->current_Pos++;

    if(Lexer->current_Pos == Lexer->text_lenght)
    {
        Lexer->current_Char = NULL;
    }
    else
    {
        Lexer->current_Char++;
    }
}

token Lexer_Number(void* lexerPtr)
{
    token   Token;

    lexer* Lexer;
    Lexer = (lexer*)lexerPtr;

    uint8_t     Number[20];
    uint8_t*    Number_Ptr;

    Number_Ptr = Number;
    memset((void*)Number,0,sizeof(Number));

    while(isdigit(*Lexer->current_Char))
    {
        *(Number_Ptr++) = *Lexer->current_Char;
        Lexer_Advance(Lexer);
    }

    Token.type = INTEGER_CONST;
    Token.value.integer = atoi((const char*)Number);

    return Token;
}

token Lexer_GetNextToken(void* lexerPtr)
{
	token	Token;

	lexer* Lexer;
	Lexer = (lexer*)lexerPtr;

	if(isdigit(*Lexer->current_Char))
	{
	    Token = Lexer_Number(Lexer);
	}

	return Token;
}
