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
#include "IrIfStmt.h"
#include "IrExpression.h"
#include "IrBlock.h"
#include "IrTravCtx.h"

namespace Decaf
{

void IrIfStatement::clean(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    m_condition->clean(ctx);
    if (m_trueBlock) m_trueBlock->clean(ctx);
    if (m_falseBlock) m_falseBlock->clean(ctx);
    
    ctx->popParent();    
}
    
void IrIfStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "If(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    m_condition->print(depth+1);
    if (m_trueBlock) m_trueBlock->print(depth+1);
    if (m_falseBlock) m_falseBlock->print(depth+1);
}
    
bool IrIfStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!m_condition->analyze(ctx))
        valid = false;
    
    if (m_trueBlock)
    {
        if (!m_trueBlock->analyze(ctx))
            valid = false;
    }
    if (m_falseBlock)
    {
        if (!m_falseBlock->analyze(ctx))
            valid = false;
    }
    
    // Condition express must be of type boolean.
    if (m_condition->getType() != IrType::Boolean)
    {
        std::stringstream msg;        
        msg <<  "if conditional expression must be of type boolean.  Got: " << IrTypeToString(m_condition->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}
    
} // namespace Decaf
