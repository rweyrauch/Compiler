#include <cstdlib>
#include <iostream>
#include <string>
#include <popt.h>
#include "Parser.h"

char* g_target = 0;
char* g_outputFilename = 0;
int g_debug = 0;
int g_opt_global_cse = 0;
int g_opt_basic_blocks = 0;
int g_opt_basic_blocks_const_prop = 0;
int g_opt_basic_blocks_cse = 0;
int g_opt_basic_blocks_alg_simp = 0;
int g_opt_basic_blocks_copy_prop = 0;
int g_opt_basic_blocks_dead_code = 0;
int g_opt_all = 0;
int g_output_ir = 0;
int g_output_blocks = 0;

poptOption appOptions[] =
{
    { "target", 't', POPT_ARG_STRING, &g_target, 0, "scan or parse", NULL },
    { "output", 'o', POPT_ARG_STRING, &g_outputFilename, 0, "output filename", NULL },
    { "debug", 'd', POPT_ARG_NONE, &g_debug, 0, "debugging output", NULL },
    { "ir", 'i', POPT_ARG_NONE, &g_output_ir, 0, "output intermediate representation", NULL },
    { "blocks", 'b', POPT_ARG_NONE, &g_output_blocks, 0, "output basic blocks", NULL },
    { "opt-common-subexpr-elim", 0, POPT_ARG_NONE, &g_opt_global_cse, 0, "enable global common subexpression elimination", NULL },
    { "opt-basic-blocks", 0, POPT_ARG_NONE, &g_opt_basic_blocks, 0, "enable all basic-blocks optimizations", NULL },
    { "opt-basic-blocks-const-prop", 0, POPT_ARG_NONE, &g_opt_basic_blocks_const_prop, 0, "enable basic-block constant propagation", NULL },
    { "opt-basic-blocks-common-subexpr-elim", 0, POPT_ARG_NONE, &g_opt_basic_blocks_cse, 0, "enable basic-block common subexpression elimination", NULL },
    { "opt-basic-blocks-alg-simp", 0, POPT_ARG_NONE, &g_opt_basic_blocks_alg_simp, 0, "enable basic-block algebraic simplification", NULL },
    { "opt-basic-blocks-copy-prop", 0, POPT_ARG_NONE, &g_opt_basic_blocks_copy_prop, 0, "enable basic-block copy propagation", NULL },
    { "opt-basic-blocks-dead-code", 0, POPT_ARG_NONE, &g_opt_basic_blocks_dead_code, 0, "enable basic-block dead code elimination", NULL },
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
            parser = new Parser(poptGetArg(cmd), g_outputFilename ? g_outputFilename : "decaf.out");
        }
        else
        {
            parser = new Parser(std::cin, std::cout);
        }
        
        if (g_output_ir) parser->enableIrOutput();
        if (g_output_blocks) parser->enableBasicBlocksOutput();
        
        if (g_opt_basic_blocks) parser->enableOpt(Optimization::BASIC_BLOCKS_ALL);
        if (g_opt_basic_blocks_const_prop) parser->enableOpt(Optimization::BASIC_BLOCKS_CONST_PROP);
        if (g_opt_basic_blocks_cse) parser->enableOpt(Optimization::BASIC_BLOCKS_CSE);
        if (g_opt_basic_blocks_alg_simp) parser->enableOpt(Optimization::BASIC_BLOCKS_ALG_SIMP);
        if (g_opt_basic_blocks_copy_prop) parser->enableOpt(Optimization::BASIC_BLOCKS_COPY_PROP);
        if (g_opt_basic_blocks_dead_code) parser->enableOpt(Optimization::BASIC_BLOCKS_DEAD_CODE);
        
        if (g_opt_global_cse) parser->enableOpt(Optimization::GLOBAL_CSE);
        if (g_opt_all) parser->enableOpt(Optimization::ALL);
        
        parser->parse();        
        if (parser->semanticChecks())
        {
            if (g_debug)
                parser->dumpAST();
            parser->codegen();
        }
        
        delete parser;
    }
    return 0;
}
