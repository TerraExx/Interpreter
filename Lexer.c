#include "Common.h"
#include "Lexer.h"

#include <ctype.h>

uint8_t KeyWordString[KEY_WORD_NUM][KEY_WORD_MAX_LENGHT] =
{
        "variables",
		"main",
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
        KeyWordArray[Idx].keyword_token.type = (e_lexer_token_type) Idx;
        strcpy((char*)KeyWordArray[Idx].keyword_token.value.string,(const char*)KeyWordString[Idx]);
    }
}

void Lexer_Init(s_lexer_lexer* Lexer, uint8_t* text)
{
	Lexer->text = text;

	Lexer->current_Pos = 0;
	Lexer->current_Char = text;

	Lexer_InitKeywords();
}

void Lexer_Advance(s_lexer_lexer* Lexer)
{
    Lexer->current_Pos++;

    if(Lexer->current_Pos == strlen((const char*)Lexer->text))
    {
        Lexer->current_Char = NULL;
    }
    else
    {
        Lexer->current_Char++;
    }
}

s_lexer_token Lexer_Number(s_lexer_lexer* Lexer)
{
    s_lexer_token   Token;

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

s_lexer_token Lexer_Id(s_lexer_lexer* Lexer)
{
    s_lexer_token   Token;

    uint8_t     String[32];
    uint8_t*    String_Ptr;

    uint8_t     Idx;

    String_Ptr = String;
    memset((void*)String,0,sizeof(String));

    while((Lexer->current_Char != NULL) && (isalnum(*Lexer->current_Char) || *Lexer->current_Char == '_'))
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

s_lexer_token Lexer_GetNextToken(s_lexer_lexer* Lexer)
{
	s_lexer_token	Token;

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

	    if(isalpha(*Lexer->current_Char) || *Lexer->current_Char == '_')
	    {
	        Token = Lexer_Id(Lexer);
	        break;
	    }

	    if(*Lexer->current_Char == '=')
	    {
	        Token.type = ASSIGN;
	        strcpy(Token.value.string, (const char*)"=");
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

        if(*Lexer->current_Char == ',')
        {
            Token.type = COMMA;
            strcpy(Token.value.string, (const char*)",");
            Lexer_Advance(Lexer);
            break;
        }

        printf("Could not read token.\n");
        exit(1);

	}

	return Token;
}
