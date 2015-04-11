
digit [0-9]
xdigit [a-fA-F0-9]


%x string
%x comment

%%

"//"[^\n]*                      ;   // comment
"/*"                            { track_column(); begin(StartCondition__::comment); }

<comment>{
    "*/"                        { track_column(); begin(StartCondition__::INITIAL); }
    .|\n                        { track_column(); }; // c-style comment - skip
}

"callout"                       { track_column(); return (Parser::CALLOUT); }
"return"                        { track_column(); return (Parser::RETURN); }

"boolean"                       { track_column(); return (Parser::BOOLTYPE); }
"int"                           { track_column(); return (Parser::INTTYPE); }
"double"                        { track_column(); return (Parser::DOUBLETYPE); }
"string"                        { track_column(); return (Parser::STRINGTYPE); }
"void"                          { track_column(); return (Parser::VOID); }

"class"                         { track_column(); return (Parser::CLASS); }
"interface"                     { track_column(); return (Parser::INTERFACE); }
"null"                          { track_column(); return (Parser::NULLVALUE); }
"this"                          { track_column(); return (Parser::THIS); }
"extends"                       { track_column(); return (Parser::EXTENDS); }
"implements"                    { track_column(); return (Parser::IMPLEMENTS); }

"if"                            { track_column(); return (Parser::IF); }
"else"                          { track_column(); return (Parser::ELSE); }
"switch"                        { track_column(); return (Parser::SWITCH); }
"case"                          { track_column(); return (Parser::CASE); }
"default"                       { track_column(); return (Parser::DEFAULT); }

"for"                           { track_column(); return (Parser::FOR); }
"while"                         { track_column(); return (Parser::WHILE); }
"do"                            { track_column(); return (Parser::DO); }
"break"                         { track_column(); return (Parser::BREAK); }
"continue"                      { track_column(); return (Parser::CONTINUE); }
"goto"                          { track_column(); return (Parser::GOTO); }
"new"                           { track_column(); return (Parser::NEW); }

false|true                      { track_column(); return (Parser::BOOLEAN); }

[a-zA-Z_][a-zA-Z0-9_]*          { track_column(); return (Parser::IDENTIFIER); }
{digit}+\.{digit}*([eE]{digit})*  { track_column(); return (Parser::DOUBLE); }
0[xX]{xdigit}+                  { track_column(); return (Parser::INTEGER); }
{digit}+                        { track_column(); return (Parser::INTEGER); }
[0][xX]                         { track_column(); error(); }
'(\\[tn\\\"]|[^\\'\n\t\"])'     { track_column(); return (Parser::CHARACTER); }

\"                              { track_column(); more(); begin(StartCondition__::string); }
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

"="                             { track_column(); return (Parser::EQUAL); }
"+="                            { track_column(); return (Parser::PLUSEQUAL); }
"-="                            { track_column(); return (Parser::MINUSEQUAL); }

"=="                            { track_column(); return (Parser::CEQ); }
"!="                            { track_column(); return (Parser::CNE); }

"||"                            { track_column(); return (Parser::LOR); }
"&&"                            { track_column(); return (Parser::LAND); }

"<"                             { track_column(); return (Parser::CLT); }
"<="                            { track_column(); return (Parser::CLE); }
">"                             { track_column(); return (Parser::CGT); }
">="                            { track_column(); return (Parser::CGE); }

"("                             { track_column(); return (Parser::LPAREN); }
")"                             { track_column(); return (Parser::RPAREN); }
"{"                             { track_column(); return (Parser::LBRACE); }
"}"                             { track_column(); return (Parser::RBRACE); }
"["                             { track_column(); return (Parser::LBRACKET); }
"]"                             { track_column(); return (Parser::RBRACKET); }

","                             { track_column(); return (Parser::COMMA); }
"!"                             { track_column(); return (Parser::BANG); }
"."                             { track_column(); return (Parser::DOT); }

"+"                             { track_column(); return (Parser::PLUS); }
"-"                             { track_column(); return (Parser::MINUS); }
"*"                             { track_column(); return (Parser::MUL); }
"/"                             { track_column(); return (Parser::DIV); }
"%"                             { track_column(); return (Parser::MOD); }

";"                             { track_column(); return (Parser::SEMI); }
":"                             { track_column(); return (Parser::COLON); }

[ \t\v\f\n]                     { track_column(); } // white space

.                               { track_column(); error(); }

