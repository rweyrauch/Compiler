%{
#include <iostream>
#include "decafParser.hpp"

extern bool g_scan_only;

#define PRINT_TOKEN std::cout << g_scanner_line_num << " " << yytext << std::endl;
#define PRINT_IDENTIFIER std::cout << g_scanner_line_num << " IDENTIFIER " << yytext << std::endl;
#define PRINT_INTLITERAL std::cout << g_scanner_line_num << " INTLITERAL " << yytext << std::endl;
#define PRINT_STRINGLITERAL std::cout << g_scanner_line_num << " STRINGLITERAL " << yytext << std::endl;
#define PRINT_CHARLITERAL std::cout << g_scanner_line_num << " CHARLITERAL " << yytext << std::endl;
#define PRINT_BOOLITERAL std::cout << g_scanner_line_num << " BOOLEANLITERAL " << yytext << std::endl;
#define PRINT_COMMENT std::cout << g_scanner_line_num << " COMMENT " << yytext << std::endl;
#define ERROR std::cout << "Unknown token, \"" << yytext << "\", at column " << g_scanner_column_num << " on line " << g_scanner_line_num << std::endl;

int g_scanner_line_num = 1;
int g_scanner_column_num = 0;

void track_column();
 
%}

%option noyywrap


digit [0-9]
xdigit [a-fA-F0-9]

%%

"////"[^\n]*					;	// comment

"callout"                       { track_column(); if (g_scan_only) PRINT_TOKEN else return (CALLOUT); }
"return"				        { track_column(); if (g_scan_only) PRINT_TOKEN else return (RETURN); }

"boolean"						{ track_column(); if (g_scan_only) PRINT_TOKEN else return (BOOLTYPE); }
"int"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (INTTYPE); }
"class"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CLASS); }
"void"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (VOID); }

"if"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (IF); }
"else"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (ELSE); }

"for"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (FOR); }
"break"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (BREAK); }
"continue"						{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CONTINUE); }

false|true				        { track_column(); if (g_scan_only) PRINT_BOOLITERAL else return (BOOLEAN); }

[a-zA-Z_][a-zA-Z0-9_]*  		{ track_column(); if (g_scan_only) PRINT_IDENTIFIER else return (IDENTIFIER); }
0[xX]{xdigit}+					{ track_column(); if (g_scan_only) PRINT_INTLITERAL else return (INTEGER); }
{digit}+						{ track_column(); if (g_scan_only) PRINT_INTLITERAL else return (INTEGER); }
[0][xX]							{ track_column(); ERROR; }
'(\\.|[^\'\"\n])'				{ track_column(); if (g_scan_only) PRINT_CHARLITERAL else return (CHARACTER); }
\"(\\.|[^\"\'\n])*\"	    	{ track_column(); if (g_scan_only) PRINT_STRINGLITERAL else return (STRING); }

"="						        { track_column(); if (g_scan_only) PRINT_TOKEN else return (EQUAL); }
"+="							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (PLUSEQUAL); }
"-="							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (MINUSEQUAL); }

"=="				          	{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CEQ); }
"!="			          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CNE); }

"||"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (LOR); }
"&&"							{ track_column(); if (g_scan_only) PRINT_TOKEN else return (LAND); }

"<"				          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CLT); }
"<="	          				{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CLE); }
">"				          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (CGT); }
">="					        { track_column(); if (g_scan_only) PRINT_TOKEN else return (CGE); }

"("	          					{ track_column(); if (g_scan_only) PRINT_TOKEN else return (LPAREN); }
")"					          	{ track_column(); if (g_scan_only) PRINT_TOKEN else return (RPAREN); }
"{"         					{ track_column(); if (g_scan_only) PRINT_TOKEN else return (LBRACE); }
"}"					          	{ track_column(); if (g_scan_only) PRINT_TOKEN else return (RBRACE); }
"["								{ track_column(); if (g_scan_only) PRINT_TOKEN else return (LBRACKET); }
"]"								{ track_column(); if (g_scan_only) PRINT_TOKEN else return (RBRACKET); }

","				          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (COMMA); }
"!"								{ track_column(); if (g_scan_only) PRINT_TOKEN else return (BANG); }

"+"				          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (PLUS); }
"-"		          				{ track_column(); if (g_scan_only) PRINT_TOKEN else return (MINUS); }
"*"		          				{ track_column(); if (g_scan_only) PRINT_TOKEN else return (MUL); }
"/"				          		{ track_column(); if (g_scan_only) PRINT_TOKEN else return (DIV); }
"%"							    { track_column(); if (g_scan_only) PRINT_TOKEN else return (MOD); }

";"								{ track_column(); if (g_scan_only) PRINT_TOKEN else return (SEMI); }

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
