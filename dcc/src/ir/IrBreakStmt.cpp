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
#include "IrBreakStmt.h"
#include "IrTravCtx.h"
#include "IrForStmt.h"
#include "IrWhileStmt.h"
#include "IrDoWhileStmt.h"
#include "IrIdentifier.h"

namespace Decaf
{
    
void IrBreakStatement::propagateTypes(IrTraversalContext* ctx)
{
    // nothing to do
}
    
void IrBreakStatement::print(unsigned int depth)
{
    IRPRINT_INDENT(depth);
    std::cout << "Break(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
}
        
bool IrBreakStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = false;
    
    // Walk up the parent list, looking for a loop (for, while, do-while) statement.
    for (size_t i = 0; i < ctx->getNumParents(); i++)
    {
        const IrForStatement* forloop = dynamic_cast<const IrForStatement*>(ctx->getParent(i));
        const IrWhileStatement* whileloop = dynamic_cast<const IrWhileStatement*>(ctx->getParent(i));
        const IrDoWhileStatement* doloop = dynamic_cast<const IrDoWhileStatement*>(ctx->getParent(i));
        if (forloop || whileloop || doloop)
        {
            m_parentLoop = forloop;
            valid = true;
            break;
        }
    }
    
    if (!valid)
    {
        ctx->error(this, "break statement not found in a loop.");
    }
    return valid;
}

bool IrBreakStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = false;
    
    if (m_parentLoop != nullptr)
    {
        IrTacStmt jump(IrOpcode::JUMP, getLineNumber());
        jump.m_src0.build(m_parentLoop->getLoopEnd().get());
        ctx->append(jump);
    }
    
    return valid;
}

} // namespace Decaf
