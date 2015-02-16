// Generated by Flexc++ V1.08.00 on Sat, 14 Feb 2015 14:45:53 -0800

#ifndef Scanner_H_INCLUDED_
#define Scanner_H_INCLUDED_

#include "IrBase.h"
#include "IrBlock.h"
#include "IrClass.h"

using namespace Decaf;

// $insert baseclass_h
#include "Scannerbase.h"


// $insert classHead
class Scanner: public ScannerBase
{
    public:
        explicit Scanner(std::istream &in = std::cin,
                         std::ostream &out = std::cout);

        Scanner(std::string const &infile, std::string const &outfile);
        
        // $insert lexFunctionDecl
        int lex();

        int columnNr() { return m_current_column; }
        
    protected:
    
        int m_current_column;
        
        void error();
        void track_column();
        
    private:
        int lex__();
        int executeAction__(size_t ruleNr);

        void print();
        void preCode();     // re-implement this function for code that must 
                            // be exec'ed before the patternmatching starts

        void postCode(PostEnum__ type);    
                            // re-implement this function for code that must 
                            // be exec'ed after the rules's actions.
};

// $insert scannerConstructors
inline Scanner::Scanner(std::istream &in, std::ostream &out)
:
    ScannerBase(in, out),
    m_current_column(0)

{}

inline Scanner::Scanner(std::string const &infile, std::string const &outfile)
:
    ScannerBase(infile, outfile),
    m_current_column(0)
{}

// $insert inlineLexFunction
inline int Scanner::lex()
{
    return lex__();
}

inline void Scanner::preCode() 
{
    // optionally replace by your own code
}

inline void Scanner::postCode(PostEnum__ type) 
{
    // optionally replace by your own code
}

inline void Scanner::error()
{
    std::cout << filename() << " line " << lineNr() << ":" << m_current_column << ": unexpected char: \'" << matched().front() << "\'" << std::endl;    
}

inline void Scanner::track_column()
{
	const int tab_width = 4;

	for (auto it = matched().cbegin(); it != matched().cend(); ++it)
	{
		if (*it == '\n')
			m_current_column = 0;
		else if (*it == '\t')
			m_current_column += tab_width - (m_current_column % tab_width);
		else
			m_current_column++;
	}
}
 
inline void Scanner::print() 
{
    print__();
}


#endif // Scanner_H_INCLUDED_

