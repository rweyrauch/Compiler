//
// The MIT License (MIT)
//
// Copyright (c) 2015 Rick Weyrauch (rpweyrauch@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once
#include <list>
#include <vector>
#include <map>
#include <iostream>
#include "IrCommon.h"
#include "IrBase.h"
#include "IrTAC.h"

namespace Decaf
{
class IrSymbolTable;
class IrLocation;
class IrMethodCall;
class IrIdentifier;

class IrTraversalContext
{
public:
    IrTraversalContext() :
        m_symbols(),
        m_parents(),
        m_sourceFilename(""),
        m_source(nullptr),
        m_blank(""),
        m_statements(),
        m_strings(),
        m_doubles()
    {}
    
    ~IrTraversalContext() {}
    
    void pushSymbols(IrSymbolTable* symbols) { m_symbols.push_front(symbols); }
    void popSymbols() { m_symbols.pop_front(); }
    
    void pushParent(IrBase* parent) { m_parents.push_back(parent); }
    void popParent() { m_parents.pop_back(); }
    
    size_t getNumParents() const { return m_parents.size(); }
    const IrBase* getParent(size_t generation) const;
   
    bool addTempVariable(IrIdentifier* variable, IrType type);
    bool lookup(IrIdentifier* variable, SVariableSymbol& symbol) const;
    bool lookup(IrLocation* variable, SVariableSymbol& symbol) const;
    bool lookup(IrMethodCall* method, SMethodSymbol& symbol) const;
    
    bool addString(IrIdentifier* identifier, const std::string& value);
    bool lookup(const std::string& value, SStringSymbol& symbol);
    
    bool addDouble(IrIdentifier* identifier, double value);
    bool lookup(double value, SDoubleSymbol& symbol);
    
    void setSource(const std::string& filename, std::vector<std::string> const* source) { m_sourceFilename = filename; m_source = source; }
    const std::string& sourceAt(int line_num) const;
    const std::string& sourceFilename() const { return m_sourceFilename; }
    
    void error(const IrBase* node, const std::string& message) const;
    void highlightError(int line, int column, int length = 1) const;
    
    void append(const IrTacStmt& stmt)
    {
        m_statements.push_back(stmt);
    }
    const std::vector<IrTacStmt>& getStatements() const { return m_statements; }
    void setStatements(const std::vector<IrTacStmt>& statements) { m_statements = statements; }
    
    void codegen(std::ostream& stream);
    
    void genStrings();
    void genDoubles();
    
protected:
    
    std::list<IrSymbolTable*> m_symbols;
    std::vector<IrBase*> m_parents;
    
    std::string m_sourceFilename;
    std::vector<std::string> const* m_source;
    std::string m_blank;
    
    std::vector<IrTacStmt> m_statements;
    
    std::map<std::string, SStringSymbol> m_strings; 
    std::map<std::string, SDoubleSymbol> m_doubles;
    
};

} // namespace Decaf
