#include <cstdlib>

extern int yyparse();
extern int yylex();

int main(int argc, char **argv)
{
	if (argc > 1)
		yylex();
	else	
		yyparse();
	
	return 0;
}
