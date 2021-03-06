#include "System.h"
#include "Lexer.h"

#include <ctype.h>

uint8_t KeyWordString[KEYWORD_NUM][KEY_WORD_MAX_LENGHT] =
{
        "variables",
		"main",
		"return",
		"if",
		"else",
		"for",
		"while",
		"break",
		"TestWaitForTimeout",
        "int8",
        "int16",
        "int32",
        "uint8",
        "uint16",
        "uint32",
        "float",
        "void"
};

uint8_t TokenTypeString[TOKEN_TYPE_NUM][KEY_WORD_MAX_LENGHT] =
{
        /* Key Words */
        "VARIABLES",
        "MAIN",
        "RETURN",
        "IF",
        "ELSE",
        "FOR",
        "WHILE",
        "BREAK",
        "TW_TIMEOUT",
        /* Key Words - Built-in Types */
        "INT8",
        "INT16",
        "INT32",
        "UINT8",
        "UINT16",
        "UINT32",
        "FLOAT",
        "VOID",

        /* Other Tokens */
        "ELSE_IF",
        "INTEGER_CONST",
        "REAL_CONST",
        "ID",
        "LPAREN",
        "RPAREN",
        "LCURLY",
        "RCURLY",
        "ASSIGN",
        "COMMA",
        "SEMICOL",
        "PLUS",
        "MINUS",
        "MUL",
        "DIV",
        "LESS_THAN",
        "LT_EQUAL",
        "GREATER_THAN",
        "GT_EQUAL",
        "EQUAL",
};

s_lexer_keyword  KeyWordArray[KEYWORD_NUM];

void Lexer_InitKeywords(void)
{
    uint8_t Idx;

    for(Idx = 0; Idx < KEYWORD_NUM; Idx++)
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
	Lexer->line = 1;

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

uint8_t Lexer_Peek( uint8_t* tokenText, s_lexer_lexer* Lexer )
{
    uint8_t     Ret;
    uint8_t*    PeekChar;
    uint16_t    PeekPos;

    /* Init */
    Ret = FALSE;
    PeekPos = Lexer->current_Pos;
    PeekChar = Lexer->current_Char;

    /* Skip white space */
    while((*PeekChar == ' ') && (PeekPos != strlen((const char*)Lexer->text)))
    {
        PeekPos++;
        PeekChar++;
    }

    /* Compare tokenText with actual text */
    if( *PeekChar == *tokenText )
    {
        Ret = TRUE;
    }

    return Ret;
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


    for(Idx = 0; Idx < KEYWORD_NUM; Idx++)
    {
        if(!strcmp((const char*)String,(const char*)KeyWordArray[Idx].string))
        {
            break;
        }
    }

    if(Idx == KEYWORD_NUM)
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
	s_lexer_token	    Token;

	while(Lexer->current_Char != NULL)
	{
        if(*Lexer->current_Char == '\n')
        {
            Lexer->line++;
            Lexer_Advance(Lexer);
            continue;
        }

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

        if(*Lexer->current_Char == '=' && *(Lexer->current_Char+1) == '=')
        {
            Token.type = EQUAL;
            strcpy(Token.value.string, (const char*)"==");
            Lexer_Advance(Lexer);
            Lexer_Advance(Lexer);
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

        if(*Lexer->current_Char == '<' && *(Lexer->current_Char+1) == '=')
        {
            Token.type = LT_EQUAL;
            strcpy(Token.value.string, (const char*)"<=");
            Lexer_Advance(Lexer);
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '<')
        {
            Token.type = LESS_THAN;
            strcpy(Token.value.string, (const char*)"<");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '>' && *(Lexer->current_Char+1) == '=')
        {
            Token.type = GT_EQUAL;
            strcpy(Token.value.string, (const char*)">=");
            Lexer_Advance(Lexer);
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '>')
        {
            Token.type = GREATER_THAN;
            strcpy(Token.value.string, (const char*)">");
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

        if(*Lexer->current_Char == '*')
        {
            Token.type = MUL;
            strcpy(Token.value.string, (const char*)"*");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '/')
        {
            Token.type = DIV;
            strcpy(Token.value.string, (const char*)"/");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == '(')
        {
            Token.type = LPAREN;
            strcpy(Token.value.string, (const char*)"(");
            Lexer_Advance(Lexer);
            break;
        }

        if(*Lexer->current_Char == ')')
        {
            Token.type = RPAREN;
            strcpy(Token.value.string, (const char*)")");
            Lexer_Advance(Lexer);
            break;
        }

        printf("Could not read token.\n");
        exit(1);

	}

	/* Record Token Line */
	Token.line = Lexer->line;

	return Token;
}
