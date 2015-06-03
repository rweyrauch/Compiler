// Generated by Bisonc++ V4.05.00 on Sat, 14 Feb 2015 14:45:53 -0800

#ifndef Parser_h_included
#define Parser_h_included

#include <algorithm>
#include <fstream>
#include "Ir.h"

using namespace Decaf;

// $insert baseclass
#include "Parserbase.h"
// $insert scanner.h
#include "Scanner.h"

enum class Optimization : int
{
    BASIC_BLOCKS_CONST_FOLDING,
    BASIC_BLOCKS_ALG_SIMP,
    BASIC_BLOCKS_CSE,
    BASIC_BLOCKS_COPY_PROP,
    BASIC_BLOCKS_DEAD_CODE,
    BASIC_BLOCKS_ALL,
    GLOBAL_CSE,
    ALL
};

#undef Parser
class Parser: public ParserBase
{
    // $insert scannerobject
    Scanner d_scanner;

    IrClass* d_root;
    IrTraversalContext* d_ctx;
    IrOptimizer* d_optimizer;
    
    std::vector<std::string> d_source;
    std::string d_blank;
    
    std::vector<Optimization> m_optimizations;
    IrBasicBlockOpts m_blockOpts;
    bool m_enableIrOutput;
    bool m_enableBasicBlocksOutput;
    
    public:
        
        Parser(std::istream &in, std::ostream &out) :
            d_scanner(in, out),
            d_root(nullptr),
            d_ctx(nullptr),
            d_optimizer(nullptr),
            m_blockOpts(BBOPTS_NONE),
            m_enableIrOutput(false),
            m_enableBasicBlocksOutput(false)
        {
            d_scanner.setSLoc(&d_loc__);
            d_ctx = new IrTraversalContext();
            d_optimizer = new IrOptimizer();
        }
        Parser(std::string const &infile, std::string const &outfile) :
            d_scanner(infile, outfile),
            d_root(nullptr),
            d_ctx(nullptr),
            d_optimizer(nullptr),
            m_blockOpts(BBOPTS_NONE),            
            m_enableIrOutput(false),
            m_enableBasicBlocksOutput(false)
       {
            preloadSource(infile);            
            d_scanner.setSLoc(&d_loc__);            
            d_ctx = new IrTraversalContext();
            d_ctx->setSource(infile, &d_source);
            d_optimizer = new IrOptimizer();
        }
        virtual ~Parser() 
        {
        }
        void enableOpt(Optimization which)
        {
            if (which == Optimization::ALL)
            {
                m_blockOpts = BBOPTS_ALL;
                m_optimizations.push_back(Optimization::GLOBAL_CSE);
            }
            else if (which == Optimization::BASIC_BLOCKS_CONST_FOLDING)
            {
                 m_blockOpts = static_cast<IrBasicBlockOpts>(m_blockOpts | BBOPTS_CONSTANT_FOLDING);
            }
            else if (which == Optimization::BASIC_BLOCKS_ALG_SIMP)
            {
                m_blockOpts = static_cast<IrBasicBlockOpts>(m_blockOpts | BBOPTS_ALGEBRAIC_SIMP);
            }
            else if (which == Optimization::BASIC_BLOCKS_CSE)
            {
                m_blockOpts = static_cast<IrBasicBlockOpts>(m_blockOpts | BBOPTS_COMMON_SUBEXPR_ELIM);
            }
            else if (which == Optimization::BASIC_BLOCKS_COPY_PROP)
            {
                m_blockOpts = static_cast<IrBasicBlockOpts>(m_blockOpts | BBOPTS_COPY_PROP);
            }
            else if (which == Optimization::BASIC_BLOCKS_DEAD_CODE)
            {
                m_blockOpts = static_cast<IrBasicBlockOpts>(m_blockOpts | BBOPTS_DEAD_CODE_ELIM);
            }
            else
            {
                m_optimizations.push_back(which);
            }
        }
        void enableIrOutput()
        {
            m_enableIrOutput = true;
        }
        void enableBasicBlocksOutput()
        {
            m_enableBasicBlocksOutput = true;
        }
        int parse();
        bool codegen()
        {
            if (d_root)
            {
                d_root->allocate(d_ctx);
    
                d_root->codegen(d_ctx);
                
                // convert generated TAC into basic blocks and optimize
                std::vector<IrTacStmt> statements = optimize(d_ctx->getStatements());
                
                d_ctx->setStatements(statements);
                
                // write string table
                d_ctx->genStrings();

                // write double constants
                d_ctx->genDoubles();
                
                if (m_enableIrOutput)
                {
                    for (auto it : statements)
                    {
                        IrPrintTac(it);
                    }
                }
    
                // convert TAC into x86_64 assembly
                d_ctx->codegen(d_scanner.outStream());
            }
            return false;
        }
        void dumpAST() 
        { 
            if (d_root) 
                d_root->print(0); 
        }
        bool semanticChecks() 
        { 
            if (d_root) 
            {
                d_root->propagateTypes(d_ctx);
                return d_root->analyze(d_ctx); 
            }
            return false; 
        }
        std::vector<IrTacStmt> optimize(const std::vector<IrTacStmt>& statements)
        {
            d_optimizer->generateBasicBlocks(statements);
            
            std::sort(m_optimizations.begin(), m_optimizations.end());
            std::unique(m_optimizations.begin(), m_optimizations.end());
            
            d_optimizer->basicBlocksOptimizations(m_blockOpts);                    
            
            // apply requested optimizations in the required order
            for (auto it : m_optimizations)
            {
                if (it == Optimization::GLOBAL_CSE)
                {
                    d_optimizer->globalCommonSubexpressionElimination();
                }
            }
            
            if (m_enableBasicBlocksOutput) d_optimizer->print();
                        
            return d_optimizer->getOptimizedStatements();
        }
        
    private:
        void error(char const *msg);    // called on (syntax) errors
        int lex();                      // returns the next token from the
                                        // lexical scanner. 
        void print();                   // use, e.g., d_token, d_loc

        void setRoot(IrClass* root) { d_root = root; }
        
        const std::string& line(size_t line_num) const;
        
        void preloadSource(const std::string &infile);

    // support functions for parse():
        void executeAction(int ruleNr);
        void errorRecovery();
        int lookup(bool recovery);
        void nextToken();
        void print__();
        void exceptionHandler__(std::exception const &exc);
};


#endif
