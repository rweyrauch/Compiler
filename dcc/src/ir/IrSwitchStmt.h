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
#include <memory>
#include "IrCommon.h"
#include "IrStatement.h"
#include "IrCaseStmt.h"
#include "IrExpression.h"

namespace Decaf
{

class IrSwitchStatement : public IrStatement
{
public:
    IrSwitchStatement(int lineNumber, int columnNumber, const std::string& filename, IrExpression* expr) :
        IrStatement(lineNumber, columnNumber, filename),
        m_expression(expr)
    {}
    
    virtual ~IrSwitchStatement()
    {}
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth); 
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_switch; }
  
    void addStatement(IrCaseStatement* stmt)
    {
        m_statements.push_back(std::shared_ptr<IrCaseStatement>(stmt));
    }
    void addStatements(const std::vector<IrCaseStatement*>& statements)
    {
        for (auto it : statements)
        {
            addStatement(it);
        }
    }
   
 protected:    
    
    std::shared_ptr<IrExpression> m_expression;
    std::vector<std::shared_ptr<IrCaseStatement>> m_statements;
    
    const std::string m_switch = "switch";
    
private:
    IrSwitchStatement() = delete;
    IrSwitchStatement(const IrSwitchStatement& rhs) = delete;
};

} // namespace Decaf
