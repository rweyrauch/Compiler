// Generated by Bisonc++ V4.05.00 on Sat, 14 Feb 2015 14:45:53 -0800

#ifndef Parser_h_included
#define Parser_h_included

#include "IrCommon.h"
#include "IrBase.h"
#include "IrBlock.h"
#include "IrClass.h"
#include "IrIdentifier.h"

#include "IrMethodDecl.h"
#include "IrFieldDecl.h"
#include "IrVarDecl.h"

#include "IrBinaryExpr.h"
#include "IrBooleanExpr.h"

#include "IrAssignStmt.h"
#include "IrBreakStmt.h"
#include "IrContinueStmt.h"
#include "IrExprStmt.h"
#include "IrForStmt.h"
#include "IrIfStmt.h"
#include "IrMethodCallStmt.h"
#include "IrPlusAssignStmt.h"
#include "IrReturnStmt.h"

#include "IrLiteral.h"
#include "IrIntLiteral.h"
#include "IrBoolLiteral.h"
#include "IrCharLiteral.h"

using namespace Decaf;

// $insert baseclass
#include "Parserbase.h"
// $insert scanner.h
#include "Scanner.h"


#undef Parser
class Parser: public ParserBase
{
    // $insert scannerobject
    Scanner d_scanner;

    IrClass* d_root;
    
    public:
        Parser() :
            d_scanner(),
            d_root(0) {}
        Parser(std::string const &infile, std::string const &outfile) :
            d_scanner(infile, outfile),
            d_root(0) {}
        virtual ~Parser() {}
        int parse();

        void dumpAST() { if (d_root) d_root->print(); }
        
    private:
        void error(char const *msg);    // called on (syntax) errors
        int lex();                      // returns the next token from the
                                        // lexical scanner. 
        void print();                   // use, e.g., d_token, d_loc

        void setRoot(IrClass* root) { d_root = root; }
        
    // support functions for parse():
        void executeAction(int ruleNr);
        void errorRecovery();
        int lookup(bool recovery);
        void nextToken();
        void print__();
        void exceptionHandler__(std::exception const &exc);
};


#endif
