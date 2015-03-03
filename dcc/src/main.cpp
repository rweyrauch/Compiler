#include <cstdlib>
#include <iostream>
#include <string>
#include <popt.h>
#include "Parser.h"

char* g_target = 0;
char* g_outputFilename = 0;
int g_debug = 0;

poptOption appOptions[] =
{
    { "target", 't', POPT_ARG_STRING, &g_target, 0, "scan or parse", NULL },
    { "output", 'o', POPT_ARG_STRING, &g_outputFilename, 0, "output filename", NULL },
    { "debug", 'd', POPT_ARG_NONE, &g_debug, 0, "debugging output", NULL },
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
    
    if (g_target && std::string(g_target) == "scan")
    {
        g_scan_only = true;
        Scanner* scanner = 0;
        if (poptPeekArg(cmd))
        {
            scanner = new Scanner(poptGetArg(cmd), g_outputFilename ? g_outputFilename : "decaf.out");
        }
        else
        {
            scanner = new Scanner();
        }
        int token = 0;
        while ((token = scanner->lex()))
        {
            std::cout << scanner->lineNr() << " ";
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
            std::cout << scanner->matched() << std::endl;
        }
        delete scanner;
    }
    else // "parse"
    {
        Parser* parser = 0;
        if (poptPeekArg(cmd))
        {
            parser = new Parser(poptGetArg(cmd), g_outputFilename ? g_outputFilename : "decaf.out");
        }
        else
        {
            parser = new Parser(std::cin, std::cout);
        }
        
        parser->parse();        
        parser->semanticChecks();
        parser->codegen();
           
        if (g_debug)
            parser->dumpAST();
        
        delete parser;
    }
    return 0;
}