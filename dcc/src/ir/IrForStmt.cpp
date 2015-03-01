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
#include "IrForStmt.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrBlock.h"
#include "IrTravCtx.h"

namespace Decaf
{

void IrForStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopVariable->propagateTypes(ctx);
    m_initialValue->propagateTypes(ctx);
    m_terminatingValue->propagateTypes(ctx);
    
    if (m_body) m_body->propagateTypes(ctx);
    
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
        std::stringstream msg;
        msg << "for loop initial value expression must be of type integer.  Got: " << IrTypeToString(m_initialValue->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }
    if (m_terminatingValue->getType() != IrType::Integer)
    {
        std::stringstream msg;
        msg << "for loop ending value expression must be of type integer.  Got: " << IrTypeToString(m_terminatingValue->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }

    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}
  
bool IrForStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopVariable->codegen(ctx);
    m_initialValue->codegen(ctx);
    m_terminatingValue->codegen(ctx);
    
    if (m_body) m_body->codegen(ctx);
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}
  
} // namespace Decaf
