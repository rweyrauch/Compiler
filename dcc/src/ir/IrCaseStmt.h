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
#include "IrLiteral.h"

namespace Decaf
{

class IrCaseStatement : public IrStatement
{
public:
    IrCaseStatement(int lineNumber, int columnNumber, const std::string& filename, IrIntegerLiteralPtr value) :
        IrStatement(lineNumber, columnNumber, filename),
        m_isDefault(false),
        m_value(value),
        m_statements()
    {
    }
    IrCaseStatement(int lineNumber, int columnNumber, const std::string& filename) :
        IrStatement(lineNumber, columnNumber, filename),
        m_isDefault(true),
        m_value(nullptr),
        m_statements()
    {
    }
    
    virtual ~IrCaseStatement();
     
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx) { return true; }    
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_case; }
    
    void addStatement(IrStatementPtr stmt)
    {
        m_statements.push_back(stmt);
    }
    void addStatements(const std::vector<IrStatement*>& statements)
    {
        for (auto it : statements)
        {
            addStatement(IrStatementPtr(it));
        }
    }
  
    size_t getNumStatements() const { return m_statements.size(); }
    const IrStatementPtr getStatement(size_t which) const { return m_statements.at(which); }

protected:    
    
    bool m_isDefault;
    IrIntegerLiteralPtr m_value;
    std::vector<IrStatementPtr> m_statements;
    const std::string m_case = "case";
    
private:
    IrCaseStatement() = delete;
    IrCaseStatement(const IrCaseStatement& rhs) = delete;
};

} // namespace Decaf
