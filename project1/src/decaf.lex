%{
#include <iostream>
#include "decafParser.hpp"

#define PRINT_TOKEN std::cout << g_scanner_line_num << " " << yytext << std::endl
#define PRINT_IDENTIFIER std::cout << g_scanner_line_num << " IDENTIFIER " << yytext << std::endl;
#define PRINT_INTLITERAL std::cout << g_scanner_line_num << " INTLITERAL " << yytext << std::endl;
#define PRINT_STRINGLITERAL std::cout << g_scanner_line_num << " STRINGLITERAL " << yytext << std::endl;
#define PRINT_CHARLITERAL std::cout << g_scanner_line_num << " CHARLITERAL " << yytext << std::endl;
#define PRINT_BOOLITERAL std::cout << g_scanner_line_num << " BOOLEANLITERAL " << yytext << std::endl;
#define PRINT_COMMENT std::cout << g_scanner_line_num << " COMMENT " << yytext << std::endl;
#define ERROR std::cout << "Unknown token, \"" << yytext << "\", at column " << g_scanner_column_num << " on line " << g_scanner_line_num << std::endl; YY_FLUSH_BUFFER;

int g_scanner_line_num = 1;
int g_scanner_column_num = 0;

void track_column();
 
%}

%option noyywrap

digit [0-9]
xdigit [a-fA-F0-9]

%%

"//"[^\n]*						;	// comment

"callout"                       { track_column(); PRINT_TOKEN; return (CALLOUT); }
"return"				        { track_column(); PRINT_TOKEN; return (RETURN); }

"boolean"						{ track_column(); PRINT_TOKEN; return (BOOLTYPE); }
"int"							{ track_column(); PRINT_TOKEN; return (INTTYPE); }
"class"							{ track_column(); PRINT_TOKEN; return (CLASS); }
"void"							{ track_column(); PRINT_TOKEN; return (VOID); }

"if"							{ track_column(); PRINT_TOKEN; return (IF); }
"else"							{ track_column(); PRINT_TOKEN; return (ELSE); }

"for"							{ track_column(); PRINT_TOKEN; return (FOR); }
"break"							{ track_column(); PRINT_TOKEN; return (BREAK); }
"continue"						{ track_column(); PRINT_TOKEN; return (CONTINUE); }

false|true				        { track_column(); PRINT_BOOLITERAL; return (BOOLEAN); }

[a-zA-Z_][a-zA-Z0-9_]*  		{ track_column(); PRINT_IDENTIFIER; return (IDENTIFIER); }
0[xX]{xdigit}+					{ track_column(); PRINT_INTLITERAL; return (INTEGER); }
{digit}+						{ track_column(); PRINT_INTLITERAL; return (INTEGER); }
[0][xX]							{ track_column(); ERROR; }
'(\\.|[^\\'\"\n])+'				{ track_column(); PRINT_CHARLITERAL; return (CHARACTER); }
\"(\\.|[^\\"\'\n])*\"	    	{ track_column(); PRINT_STRINGLITERAL; return (STRING); }

"="						        { track_column(); PRINT_TOKEN; return (EQUAL); }
"+="							{ track_column(); PRINT_TOKEN; return (PLUSEQUAL); }
"-="							{ track_column(); PRINT_TOKEN; return (MINUSEQUAL); }

"=="				          	{ track_column(); PRINT_TOKEN; return (CEQ); }
"!="			          		{ track_column(); PRINT_TOKEN; return (CNE); }

"||"							{ track_column(); PRINT_TOKEN; return (LOR); }
"&&"							{ track_column(); PRINT_TOKEN; return (LAND); }

"<"				          		{ track_column(); PRINT_TOKEN; return (CLT); }
"<="	          				{ track_column(); PRINT_TOKEN; return (CLE); }
">"				          		{ track_column(); PRINT_TOKEN; return (CGT); }
">="					        { track_column(); PRINT_TOKEN; return (CGE); }

"("	          					{ track_column(); PRINT_TOKEN; return (LPAREN); }
")"					          	{ track_column(); PRINT_TOKEN; return (RPAREN); }
"{"         					{ track_column(); PRINT_TOKEN; return (LBRACE); }
"}"					          	{ track_column(); PRINT_TOKEN; return (RBRACE); }
"["								{ track_column(); PRINT_TOKEN; return (LBRACKET); }
"]"								{ track_column(); PRINT_TOKEN; return (RBRACKET); }

"."         					{ track_column(); PRINT_TOKEN; return (DOT); }
","				          		{ track_column(); PRINT_TOKEN; return (COMMA); }
"!"								{ track_column(); PRINT_TOKEN; return (BANG); }

"+"				          		{ track_column(); PRINT_TOKEN; return (PLUS); }
"-"		          				{ track_column(); PRINT_TOKEN; return (MINUS); }
"*"		          				{ track_column(); PRINT_TOKEN; return (MUL); }
"/"				          		{ track_column(); PRINT_TOKEN; return (DIV); }
"%"							    { track_column(); PRINT_TOKEN; return (MOD); }

";"								{ track_column(); PRINT_TOKEN; return (SEMI); }

[ \t\v\f]					    { track_column(); } // white space
\n								{ track_column(); ++g_scanner_line_num; } // new line

.                       		{ ERROR; }

%%

void track_column()
{
	const int tab_width = 4;
	int i;
 
	for (i = 0; yytext[i] != '\0'; i++)
	{
		if (yytext[i] == '\n')
			g_scanner_column_num = 0;
		else if (yytext[i] == '\t')
			g_scanner_column_num += tab_width - (g_scanner_column_num % tab_width);
		else
			g_scanner_column_num++;
	}
}
