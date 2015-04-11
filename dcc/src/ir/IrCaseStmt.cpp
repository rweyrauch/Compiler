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
#include "IrCaseStmt.h"
#include "IrTravCtx.h"

namespace Decaf
{

 IrCaseStatement::~IrCaseStatement()
{
}

void IrCaseStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    for (auto it : m_statements)
    {
        it->propagateTypes(ctx);
    }
    
    ctx->popParent();
}

void IrCaseStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Case(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    for (auto it : m_statements)
    {
        it->print(depth+1);
    }
}
    
bool IrCaseStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    for (auto it : m_statements)
    {
        if (!it->analyze(ctx))
            valid = false;
    }   
    
    ctx->popParent();
    
    return valid;
}
    
bool IrCaseStatement::codegen(IrTraversalContext* ctx) 
{
    bool valid = true;
    ctx->pushParent(this);
    
    for (auto it : m_statements)
    {
        if (!it->codegen(ctx)) valid = false;
    }
    
    ctx->popParent();
    
    return valid; 
}
    
} // namespace Decaf
