#include <cstdlib>
#include <iostream>
#include <string>
#include <popt.h>
#include "Parser.h"
//#include "Scanner.h"

char* g_target = 0;

poptOption appOptions[] =
{
    { "target", 't', POPT_ARG_STRING|POPT_ARGFLAG_ONEDASH, &g_target, 0, "scan or parse", NULL },
    POPT_AUTOHELP
    POPT_TABLEEND
};

bool g_scan_only = false;

int main(int argc, char **argv)
{
    poptContext cmd = poptGetContext("parser", argc, (const char**)argv, appOptions, 0);
    int res = poptGetNextOpt(cmd);
    if (res < -1)
    {
        std::cerr << poptStrerror(res) << std::endl;
    }
    
	if (std::string(g_target) == "scan")
    {
        g_scan_only = true;
		Scanner scanner;
        int token = 0;
		while (token = scanner.lex())
		{
			std::cout << scanner.lineNr() << " ";
            switch (token)
            {
                case Parser::IDENTIFIER:
                    std::cout << "IDENTIFIER ";
                    break;
                case Parser::INTEGER:
                    std::cout << "INTLITERAL ";
                    break;
                case Parser::CHARACTER:
                    std::cout << "CHARLITERAL ";  
                    break;
                case Parser::STRING:
                    std::cout << "STRINGLITERAL ";
                    break;
                case Parser::BOOLEAN:
                    std::cout << "BOOLEANLITERAL ";
                    break;
            }
            std::cout << scanner.matched() << std::endl;
		}
    }
	else // "parse"
    {
		Parser parser;
		parser.parse();
	}
	return 0;
}
