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
#include <iostream>
#include <sstream>
#include "IrCommon.h"
#include "IrDoWhileStmt.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrBooleanExpr.h"
#include "IrBlock.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrDoWhileStatement::IrDoWhileStatement(int lineNumber, int columnNumber, const std::string& filename, IrExpression* loopExpr, IrBlock* block) :
    IrStatement(lineNumber, columnNumber, filename),
    m_loopExpr(loopExpr),
    m_body(std::shared_ptr<IrBlock>(block))
{
}
    
IrDoWhileStatement::~IrDoWhileStatement()
{
}
    
void IrDoWhileStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);
    
    m_loopExpr->propagateTypes(ctx);    
    if (m_body) m_body->propagateTypes(ctx);
    
    ctx->popParent();
}
    
void IrDoWhileStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Do-while(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;

    m_loopExpr->print(depth+1);
    if (m_body) m_body->print(depth+1);
}

bool IrDoWhileStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
        
    if (!m_loopExpr->analyze(ctx))
        valid = false;
    
    if (m_body) 
    {
        if (!m_body->analyze(ctx))
            valid = false;
    }
       
    ctx->popParent();
    
    return valid;
}

bool IrDoWhileStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
     
    if (!m_loopExpr->codegen(ctx))
        valid = false;
    
    if (m_body) 
    {
        if (!m_body->codegen(ctx))
            valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}

size_t IrDoWhileStatement::getAllocationSize() const
{
    size_t allocSize = 0;
    if (m_body != nullptr)
    {
        allocSize += m_body->getAllocationSize();
    }
    return allocSize;
}

void IrDoWhileStatement::setSymbolStartAddress(size_t addr)
{
    if (m_body != nullptr)
    {
        m_body->setSymbolStartAddress(addr);
    }
}
 
} // namespace Decaf
