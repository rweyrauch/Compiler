#include <cstdlib>
#include <iostream>
#include <string>
#include <popt.h>
#include "Parser.h"

char* g_target = 0;
char* g_outputFilename = 0;
int g_debug = 0;
int g_ia32 = false;
int g_opt_global_cse = false;
int g_opt_all = false;

poptOption appOptions[] =
{
    { "target", 't', POPT_ARG_STRING, &g_target, 0, "scan or parse", NULL },
    { "output", 'o', POPT_ARG_STRING, &g_outputFilename, 0, "output filename", NULL },
    { "debug", 'd', POPT_ARG_NONE, &g_debug, 0, "debugging output", NULL },
    { "m32", '3', POPT_ARG_NONE, &g_ia32, 0, "generate 32-bit code", NULL },
    { "opt-common-subexpr-elim", 0, POPT_ARG_NONE, &g_opt_global_cse, 0, "enable global common subexpression elimination", NULL },
    { "opt-all", 0, POPT_ARG_NONE, &g_opt_all, 0, "enable all optimizations", NULL },
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
            parser = new Parser(poptGetArg(cmd), g_outputFilename ? g_outputFilename : "decaf.out", !g_ia32);
        }
        else
        {
            parser = new Parser(std::cin, std::cout, !g_ia32);
        }
        
        if (g_opt_global_cse) parser->enableOpt(Optimization::GLOBAL_CSE);
        if (g_opt_all) parser->enableOpt(Optimization::ALL);
        
        parser->parse();        
        parser->semanticChecks();
        if (g_debug)
            parser->dumpAST();
        parser->codegen();
         
        delete parser;
    }
    return 0;
}
