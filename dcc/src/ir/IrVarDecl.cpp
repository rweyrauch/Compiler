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
#include "IrVarDecl.h"
#include "IrTravCtx.h"

namespace Decaf
{

void IrVariableDecl::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    for (auto it : m_identifiers)
    {
        it->propagateTypes(ctx);
    }
    
    ctx->popParent();  
}
    
void IrVariableDecl::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Variable Decl(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Type: " << IrTypeToString(m_type) << std::endl;
    
    IRPRINT_INDENT(depth+1); 
    std::cout << "Identifiers: " << std::endl;
    for (auto it : m_identifiers)
    {
        it->print(depth+2);
    }
}
 
bool IrVariableDecl::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    for (auto it : m_identifiers)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}
 
IrIdentifier* IrVariableDecl::getVariable(size_t which) const
{
    if (which < m_identifiers.size())
    {
        return m_identifiers.at(which);
    }
    return nullptr;
}

bool IrVariableDecl::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    for (auto it : m_identifiers)
    {
        if (!it->codegen(ctx))
            valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}

} // namespace Decaf