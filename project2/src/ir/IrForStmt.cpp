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
#include "IrCommon.h"
#include "IrForStmt.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrBlock.h"
#include "IrTravCtx.h"

namespace Decaf
{

void IrForStatement::clean(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopVariable->clean(ctx);
    m_initialValue->clean(ctx);
    m_terminatingValue->clean(ctx);
    
    if (m_body) m_body->clean(ctx);
    
    ctx->popParent();
    ctx->popSymbols();
}
    
void IrForStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "For(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    m_loopVariable->print(depth+1);
    m_initialValue->print(depth+1);
    m_terminatingValue->print(depth+1);
    
    if (m_body) m_body->print(depth+1);
}
  
bool IrForStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    if (!m_loopVariable->analyze(ctx))
        valid = false;
    if (!m_initialValue->analyze(ctx))
        valid = false;
    if (!m_terminatingValue->analyze(ctx))
        valid = false;
    
    if (m_body) 
    {
        if (!m_body->analyze(ctx))
            valid = false;
    }
    
    // Initial variable and terminal value expressions must be of type integer.
    if (m_initialValue->getType() != IrType::Integer)
    {
        std::cerr << getFilename() << ":" << getLineNumber() << ":" << getColumnNumber() << ": error: for loop initial value expression must be of type integer.  Got: "
                  << IrTypeToString(m_initialValue->getType()) << std::endl;		
        ctx->highlightError(getLineNumber(), getColumnNumber());
        valid = false;
    }
    if (m_terminatingValue->getType() != IrType::Integer)
    {
        std::cerr << getFilename() << ":" << getLineNumber() << ":" << getColumnNumber() << ": error: for loop ending value expression must be of type integer.  Got: "
                  << IrTypeToString(m_terminatingValue->getType()) << std::endl;		
        ctx->highlightError(getLineNumber(), getColumnNumber());
        valid = false;
    }

    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}
  
} // namespace Decaf
