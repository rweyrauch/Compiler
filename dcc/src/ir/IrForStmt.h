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
class IrIdentifier;
class IrExpression;
class IrBlock;
class IrIntegerLiteral;
class IrGotoStatement;
class IrLocation;

class IrForStatement : public IrStatement
{
public:
    IrForStatement(int lineNumber, int columnNumber, const std::string& filename, 
        IrIdentifier* loopVar, IrExpression* initialExpr, IrExpression* endExpr, IrBlock* block = nullptr);
    
    virtual ~IrForStatement();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_for; }
       
    virtual size_t getAllocationSize() const;
    virtual void setSymbolStartAddress(size_t addr);
        
    std::shared_ptr<IrIdentifier> getLoopBegin() const { return m_labelTop; }
    std::shared_ptr<IrIdentifier> getLoopContinue() const { return m_labelContinue; }
    std::shared_ptr<IrIdentifier> getLoopEnd() const { return m_labelEnd; }
    
protected:    
    
    std::shared_ptr<IrLocation> m_loopVar;
    std::shared_ptr<IrExpression> m_initialValue;
    std::shared_ptr<IrExpression> m_terminatingValue;
    std::shared_ptr<IrBlock> m_body;
    
    std::shared_ptr<IrIdentifier> m_labelTop;
    std::shared_ptr<IrIdentifier> m_labelContinue;
    std::shared_ptr<IrIdentifier> m_labelEnd;
    std::shared_ptr<IrExpression> m_initLoopAuto;
    std::shared_ptr<IrExpression> m_terminatingExpr;
    std::shared_ptr<IrExpression> m_incrementLoop;
    std::shared_ptr<IrIntegerLiteral> m_loopIncrement;
    std::shared_ptr<IrGotoStatement> m_loopGoto;
    
    const std::string m_for = "for";
private:
    IrForStatement() = delete;
    IrForStatement(const IrForStatement& rhs) = delete;
};

} // namespace Decaf
