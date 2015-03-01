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
#include "IrContinueStmt.h"
#include "IrTravCtx.h"
#include "IrForStmt.h"

namespace Decaf
{
    
void IrContinueStatement::propagateTypes(IrTraversalContext* ctx)
{
    // nothing to do
}
    
void IrContinueStatement::print(unsigned int depth)
{
    IRPRINT_INDENT(depth);
    std::cout << "Continue(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;    
}
        
bool IrContinueStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = false;
    
    // Walk up the parent list, looking for a for-loop statement.
    for (size_t i = 0; i < ctx->getNumParents(); i++)
    {
        const IrForStatement* forloop = dynamic_cast<const IrForStatement*>(ctx->getParent(i));
        if (forloop)
        {
            valid = true;
            break;
        }
    }
    
    if (!valid)
    {
        ctx->error(this, "continue statement not found in a for-loop.");
    }
    
    return valid;
}

} // namespace Decaf
