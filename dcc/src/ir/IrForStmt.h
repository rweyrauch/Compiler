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

namespace Decaf
{
class IrBlock;
class IrIntegerLiteral;
class IrGotoStatement;
class IrLocation;

class IrForStatement : public IrStatement
{
public:
    IrForStatement(int lineNumber, int columnNumber, const std::string& filename, 
        IrExpressionPtr initialExpr, IrExpressionPtr endExpr, IrExpressionPtr loopExpr, IrStatementPtr block = nullptr);
    
    virtual ~IrForStatement();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_for; }
       
    virtual size_t getAllocationSize() const;
    virtual void setSymbolStartAddress(size_t addr);
        
    IrIdentifierPtr getLoopContinue() const { return m_labelContinue; }
    IrIdentifierPtr getLoopEnd() const { return m_labelEnd; }
    
protected:    
    
    IrExpressionPtr m_initialExpr;
    IrExpressionPtr m_terminatingExpr;
    IrExpressionPtr m_loopExpr;
    IrStatementPtr m_body;
    
    IrIdentifierPtr m_labelTop;
    IrIdentifierPtr m_labelContinue;
    IrIdentifierPtr m_labelEnd;
    IrGotoStatementPtr m_loopGoto;
    
    const std::string m_for = "for";
private:
    IrForStatement() = delete;
    IrForStatement(const IrForStatement& rhs) = delete;
};

} // namespace Decaf
