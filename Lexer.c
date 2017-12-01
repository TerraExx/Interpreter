#include "Common.h"
#include "Lexer.h"

#include <ctype.h>

uint8_t KeyWordString[KEY_WORD_NUM][KEY_WORD_MAX_LENGHT] =
{
        "variables",
        "int8",
        "int16",
        "int32",
        "uint8",
        "uint16",
        "uint32",
        "float"
};

void Lexer_InitKeywords(void)
{
    uint8_t Idx;

    for(Idx = 0; Idx < KEY_WORD_NUM; Idx++)
    {
        strcpy((char*)KeyWordArray[Idx].string,(const char*)KeyWordString[Idx]);
        switch(Idx)
        {
        case 0:
            KeyWordArray[Idx].keyword_token.type = VARIABLES;
            break;
        case 1:
            KeyWordArray[Idx].keyword_token.type = INT8;
            break;
        case 2:
            KeyWordArray[Idx].keyword_token.type = INT16;
            break;
        case 3:
            KeyWordArray[Idx].keyword_token.type = INT32;
            break;
        case 4:
            KeyWordArray[Idx].keyword_token.type = UINT8;
            break;
        case 5:
            KeyWordArray[Idx].keyword_token.type = UINT16;
            break;
        case 6:
            KeyWordArray[Idx].keyword_token.type = UINT32;
            break;
        case 7:
            KeyWordArray[Idx].keyword_token.type = FLOAT;
            break;
        }
        strcpy((char*)KeyWordArray[Idx].keyword_token.value.string,(const char*)KeyWordString[Idx]);
    }
}

void Lexer_Init(lexer* Lexer, uint8_t* text)
{
	Lexer->text = text;
	Lexer->text_lenght = strlen((const char*)text);

	Lexer->current_Pos = 0;
	Lexer->current_Char = text;

	Lexer->fcnGetNextToken = Lexer_GetNextToken;

	Lexer_InitKeywords();
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

    while((Lexer->current_Char != NULL) && isdigit(*Lexer->current_Char))
    {
        *(Number_Ptr++) = *Lexer->current_Char;
        Lexer_Advance(Lexer);
    }

    Token.type = INTEGER_CONST;
    Token.value.integer_const = atoi((const char*)Number);

    return Token;
}

token Lexer_Id(void* lexerPtr)
{
    token   Token;

    lexer* Lexer;

    uint8_t     String[32];
    uint8_t*    String_Ptr;

    uint8_t     Idx;

    Lexer = (lexer*)lexerPtr;

    String_Ptr = String;
    memset((void*)String,0,sizeof(String));

    while((Lexer->current_Char != NULL) && isalnum(*Lexer->current_Char))
    {
        *(String_Ptr++) = *Lexer->current_Char;
        Lexer_Advance(Lexer);
    }


    for(Idx = 0; Idx < KEY_WORD_NUM; Idx++)
    {
        if(!strcmp((const char*)String,(const char*)KeyWordArray[Idx].string))
        {
            break;
        }
    }

    if(Idx == KEY_WORD_NUM)
    {
        Token.type = ID;
        strcpy((char*)Token.value.string,(const char*)String);
    }
    else
    {
        Token = KeyWordArray[Idx].keyword_token;
    }

    return Token;
}

token Lexer_GetNextToken(void* lexerPtr)
{
	token	Token;

	lexer* Lexer;
	Lexer = (lexer*)lexerPtr;

	while(Lexer->current_Char != NULL)
	{
	    if(isspace(*Lexer->current_Char))
	    {
	        Lexer_Advance(Lexer);
	        continue;
	    }



	    if(isdigit(*Lexer->current_Char))
	    {
	        Token = Lexer_Number(Lexer);
	        break;
	    }

	    if(isalpha(*Lexer->current_Char))
	    {
	        Token = Lexer_Id(Lexer);
	        break;
	    }

	    if(*Lexer->current_Char == '=')
	    {
	        Token.type = ASSIGN;
	        strcpy(Token.value.string, (const char*)'=');
            Lexer_Advance(Lexer);
            break;
	    }

	    if(*Lexer->current_Char == '+')
	    {
            Token.type = PLUS;
            strcpy(Token.value.string, (const char*)"+");
            Lexer_Advance(Lexer);
            break;
	    }

        if(*Lexer->current_Char == '-')
        {
            Token.type = MINUS;
            strcpy(Token.value.string, (const char*)"-");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '{')
        {
            Token.type = LCURLY;
            strcpy(Token.value.string, (const char*)"{");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '}')
        {
            Token.type = RCURLY;
            strcpy(Token.value.string, (const char*)"}");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == ';')
        {
            Token.type = SEMICOL;
            strcpy(Token.value.string, (const char*)";");
            Lexer_Advance(Lexer);
            break;
        }
	}

	return Token;
}
