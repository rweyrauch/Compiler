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
class IrExpression;
class IrBlock;
class IrIdentifier;

class IrIfStatement : public IrStatement
{
public:
    IrIfStatement(int lineNumber, int columnNumber, const std::string& filename, IrExpression* condition, IrBlock* trueBlock, IrBlock* falseBlock = nullptr) :
        IrStatement(lineNumber, columnNumber, filename),
        m_condition(std::shared_ptr<IrExpression>(condition)),
        m_trueBlock(std::shared_ptr<IrBlock>(trueBlock)),
        m_falseBlock(std::shared_ptr<IrBlock>(falseBlock)),
        m_labelFalse(nullptr),
        m_labelEnd(nullptr)
    {}
    
    virtual ~IrIfStatement();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    
    virtual size_t getAllocationSize() const;
    virtual void setSymbolStartAddress(size_t addr);
    
protected:    
    
    std::shared_ptr<IrExpression> m_condition;
    std::shared_ptr<IrBlock> m_trueBlock;
    std::shared_ptr<IrBlock> m_falseBlock;
    
    std::shared_ptr<IrIdentifier> m_labelFalse;
    std::shared_ptr<IrIdentifier> m_labelEnd;
    
private:
    IrIfStatement() = delete;
    IrIfStatement(const IrIfStatement& rhs) = delete;
};

} // namespace Decaf
