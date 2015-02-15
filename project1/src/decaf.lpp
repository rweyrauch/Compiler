
digit [0-9]
xdigit [a-fA-F0-9]


%x string

%%

"//"[^\n]*						;	// comment

"callout"                       { track_column(); return (Parser::CALLOUT); }
"return"				        { track_column(); return (Parser::RETURN); }

"boolean"						{ track_column(); return (Parser::BOOLTYPE); }
"int"							{ track_column(); return (Parser::INTTYPE); }
"class"							{ track_column(); return (Parser::CLASS); }
"void"							{ track_column(); return (Parser::VOID); }

"if"							{ track_column(); return (Parser::IF); }
"else"							{ track_column(); return (Parser::ELSE); }

"for"							{ track_column(); return (Parser::FOR); }
"break"							{ track_column(); return (Parser::BREAK); }
"continue"						{ track_column(); return (Parser::CONTINUE); }

false|true				        { track_column(); return (Parser::BOOLEAN); }

[a-zA-Z_][a-zA-Z0-9_]*  		{ track_column(); return (Parser::IDENTIFIER); }
0[xX]{xdigit}+					{ track_column(); return (Parser::INTEGER); }
{digit}+						{ track_column(); return (Parser::INTEGER); }
[0][xX]							{ track_column(); error(); }
'(\\[tn\\\"]|[^\\'\n\t\"])'	    { track_column(); return (Parser::CHARACTER); }

\"	    	                    { track_column(); more(); begin(StartCondition__::string); }
<string>{
    \"                          {
                                    track_column();
                                    begin(StartCondition__::INITIAL);
                                    return (Parser::STRING);
                                }
    '{1}                        { 
                                    track_column();
                                    error();
                                    begin(StartCondition__::INITIAL);
                                }                            
    \\.|[^\\\n\t\"]             { 
                                    track_column(); 
                                    more(); 
                                }
}

"="						        { track_column(); return (Parser::EQUAL); }
"+="							{ track_column(); return (Parser::PLUSEQUAL); }
"-="							{ track_column(); return (Parser::MINUSEQUAL); }

"=="				          	{ track_column(); return (Parser::CEQ); }
"!="			          		{ track_column(); return (Parser::CNE); }

"||"							{ track_column(); return (Parser::LOR); }
"&&"							{ track_column(); return (Parser::LAND); }

"<"				          		{ track_column(); return (Parser::CLT); }
"<="	          				{ track_column(); return (Parser::CLE); }
">"				          		{ track_column(); return (Parser::CGT); }
">="					        { track_column(); return (Parser::CGE); }

"("	          					{ track_column(); return (Parser::LPAREN); }
")"					          	{ track_column(); return (Parser::RPAREN); }
"{"         					{ track_column(); return (Parser::LBRACE); }
"}"					          	{ track_column(); return (Parser::RBRACE); }
"["								{ track_column(); return (Parser::LBRACKET); }
"]"								{ track_column(); return (Parser::RBRACKET); }

","				          		{ track_column(); return (Parser::COMMA); }
"!"								{ track_column(); return (Parser::BANG); }

"+"				          		{ track_column(); return (Parser::PLUS); }
"-"		          				{ track_column(); return (Parser::MINUS); }
"*"		          				{ track_column(); return (Parser::MUL); }
"/"				          		{ track_column(); return (Parser::DIV); }
"%"							    { track_column(); return (Parser::MOD); }

";"								{ track_column(); return (Parser::SEMI); }

[ \t\v\f\n]					    { track_column(); } // white space

.                       		{ track_column(); error(); }

