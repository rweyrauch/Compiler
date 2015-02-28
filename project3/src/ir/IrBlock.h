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
#include <string>
#include <vector>
#include "IrStatement.h"
#include "IrSymbolTable.h"

namespace Decaf
{
class IrVariableDecl;

class IrBlock : public IrStatement
{
public:
    IrBlock(int lineNumber, int columnNumber, const std::string& filename) :
        IrStatement(lineNumber, columnNumber, filename),
        m_variables(),
        m_statements(),
        m_symbols(nullptr)
    {
        m_symbols = new IrSymbolTable();
    }
    
    virtual ~IrBlock();
     
    virtual void clean(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    
    void addVariableDecl(IrVariableDecl* var);
    void addVariableDecl(const std::vector<IrVariableDecl*>& variables);
    
    void addStatement(IrStatement* stmt)
    {
        m_statements.push_back(stmt);
    }
    void addStatements(const std::vector<IrStatement*>& statements)
    {
        m_statements.insert(m_statements.end(), statements.begin(), statements.end());
    }
  
    size_t getNumStatements() const { return m_statements.size(); }
    const IrStatement* getStatement(size_t which) const { return m_statements.at(which); }
    
    IrSymbolTable* getSymbols()
    {
        return m_symbols;
    }
  
protected:    
    
    std::vector<IrVariableDecl*> m_variables;
    std::vector<IrStatement*> m_statements;
    
    IrSymbolTable* m_symbols;
    
private:
    IrBlock() = delete;
    IrBlock(const IrBlock& rhs) = delete;
};

} // namespace Decaf
