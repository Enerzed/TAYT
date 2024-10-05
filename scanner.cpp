#include "scanner.hpp"
#include <fstream>
#include <cctype>
#include <sstream>

TScaner::TScaner(const char* file_name)
{
	get_data(file_name);
	set_pointer(0);
}

type_lex keyword[MAX_KEYWORD] =
{
	"int", "short", "long", "__int64", "char", "if", "else", "main", "void"
};

int IndexKeyword[MAX_KEYWORD] = 
{
	TInt, TShort, TLong, T__Int64, TChar, TIf, TElse, TMain, TVoid
};

void TScaner::set_pointer(int p)
{
	pointer = p;
}

int TScaner::get_pointer()
{
	return pointer;
}

void TScaner::print_error(const char* error,const char* text)
{
	if (text[0] == '\0')
		std::cout << error << std::endl;
	else
		std::cout << error << " " << text << std::endl;

	// exit(0);
}

int TScaner::scaner(type_lex lex)
{
	int i = 0;
	lex[0] = '\0';
start:
	while (text[pointer] == ' ' || text[pointer] == '\t' || text[pointer] == '\n')
		pointer++;
	i = 0;

	// End of program
	if (text[pointer] == '\0')
	{
		lex[i++] = '#';
		lex[i] = '\0';
		return TEnd;
	}

	// Comments one-line and multi-line
	if (text[pointer] == '/') 
	{
		pointer++;
		if (text[pointer] == '/') 
		{
			std::cout << "Encountered comment line" << std::endl;
			pointer++;
			while (text[pointer] != '\n' && text[pointer] != '\0')
				pointer++;
			goto start;
		}
		else if (text[pointer] == '*') 
		{
			std::cout << "Encountered comment block" << std::endl;
			pointer++;
			while (text[pointer] != '*' || text[pointer + 1] != '/') 
			{
				if (text[pointer] == '\0') 
				{
					print_error("Unterminated multi-line comment", "");
					return TErr;
				}
				pointer++;
			}
			pointer += 2;
			goto start;
		}
		else 
		{
			// Division
			pointer++;
			lex[i++] = '/';
			lex[i++] = '\0';
			return TDiv;
		}
	}

	// Hexadecimal constant
	if (text[pointer] == '0' && text[pointer + 1] == 'x')
	{
		lex[i++] = text[pointer++];
		lex[i++] = text[pointer++];
		if (!isxdigit(text[pointer]))
		{ // Check for at least one hex digit
			lex[i] = '\0';
			print_error("Invalid hexadecimal constant", lex);
			return TErr;
		}
		while (isxdigit(text[pointer]) && i < MAX_LEX - 1) 
		{
			lex[i++] = text[pointer++];
		}
		lex[i] = '\0';
		if (i == MAX_LEX - 1 && isxdigit(text[pointer])) 
		{
			lex[i] = '\0';
			while (isxdigit(text[pointer]))
				pointer++;
			print_error("Hexadecimal constant exceeds maximum lexeme length", lex);
			return TErr;
		}
		return TConst16;
	}

	// Decimal constants
	if (isdigit(text[pointer])) {
		while (isdigit(text[pointer]) && i < MAX_LEX - 1) 
		{ // Added length check
			lex[i++] = text[pointer++];
		}
		lex[i] = '\0';
		if (i == MAX_LEX - 1 && isdigit(text[pointer]))
		{
			while (isdigit(text[pointer]))
				pointer++;
			print_error("Decimal constant exceeds maximum lexeme length", lex);
			return TErr;
		}
		return TConst10;
	}

	// Identifiers
	if (isalpha(text[pointer]) || text[pointer] == '_') 
	{
		while ((isalnum(text[pointer]) || text[pointer] == '_') && i < MAX_LEX - 1) 
		{ // Added length check
			lex[i++] = text[pointer++];
		}
		lex[i] = '\0';
		if (i == MAX_LEX - 1 && (isalnum(text[pointer]) || text[pointer] == '_'))
		{
			while (isalnum(text[pointer]) || text[pointer] == '_')
				pointer++;
			print_error("Identifier exceeds maximum lexeme length", lex);
			return TErr;
		}

		for (int j = 0; j < MAX_KEYWORD; j++) 
		{
			if (strcmp(lex, keyword[j]) == 0) 
			{
				return IndexKeyword[j];
			}
		}
		return TIdent;
	}

	// Operators
	switch (text[pointer]) 
	{
		case ',': pointer++; lex[i++] = ','; lex[i] = '\0'; return TComma;
		case ';': pointer++; lex[i++] = ';'; lex[i] = '\0'; return TSemicolon;
		case '(': pointer++; lex[i++] = '('; lex[i] = '\0'; return TLeftBracket;
		case ')': pointer++; lex[i++] = ')'; lex[i] = '\0'; return TRightBracket;
		case '{': pointer++; lex[i++] = '{'; lex[i] = '\0'; return TLeftBrace;
		case '}': pointer++; lex[i++] = '}'; lex[i] = '\0'; return TRightBrace;
		case '[': pointer++; lex[i++] = '['; lex[i] = '\0'; return TLeftSquareBracket;
		case ']': pointer++; lex[i++] = ']'; lex[i] = '\0'; return TRightSquareBracket;
		case '+': pointer++; lex[i++] = '+'; lex[i] = '\0'; return TAdd;
		case '-': pointer++; lex[i++] = '-'; lex[i] = '\0'; return TSub;
		case '*': pointer++; lex[i++] = '*'; lex[i] = '\0'; return TMul;
		case '%': pointer++; lex[i++] = '%'; lex[i] = '\0'; return TMod;
		case '=':
			pointer++;
			lex[i++] = '=';
			if (text[pointer] == '=') 
			{
				pointer++;
				lex[i++] = '=';
				lex[i] = '\0';
				return TEq;
			}
			else 
			{
				lex[i] = '\0';
				return TEval;
			}
		case '!':
			pointer++;
			lex[i++] = '!';
			if (text[pointer] == '=') 
			{
				pointer++;
				lex[i++] = '=';
				lex[i] = '\0';
				return TNe;
			}
			// Not implementing NOT operation
			else 
			{
				lex[i] = '\0';
				print_error("Unexpected token", lex);
				return TErr;
			}
		case '>':
			pointer++;
			lex[i++] = '>';
			if (text[pointer] == '=')
			{
				pointer++;
				lex[i++] = '=';
				lex[i] = '\0';
				return TGe;
			}
			else 
			{
				lex[i] = '\0';
				return TGt;
			}
		case '<':
			pointer++;
			lex[i++] = '<';
			if (text[pointer] == '=')
			{
				pointer++;
				lex[i++] = '=';
				lex[i] = '\0';
				return TLe;
			}
			else 
			{
				lex[i] = '\0';
				return TLt;
			}
		default:
			lex[i++] = text[pointer];
			lex[i] = '\0';
			print_error("Lexical error at", lex);
			pointer++;
			return TErr;
	}
}

void TScaner::get_data(const char* file_name)
{
	std::ifstream file(file_name);
	if (file.is_open()) 
	{
		std::stringstream buffer;
		buffer << file.rdbuf();

		strncpy(text, buffer.str().c_str(), MAX_TEXT - 1);
		text[MAX_TEXT - 1] = '\0';

		std::cout << text << std::endl;
	}
	else 
	{
		print_error("Error opening file!", file_name);
		return;
	}
}