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
#include "IrFieldDecl.h"
#include "IrLocation.h"
#include "IrTravCtx.h"

namespace Decaf
{
    
void IrFieldDecl::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    m_location->propagateTypes(ctx);
    
    ctx->popParent();    
}
    
void IrFieldDecl::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Field Decl(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Type = " << IrTypeToString(m_type) << std::endl;
    
    IRPRINT_INDENT(depth+1); 
    std::cout << "Location: " << std::endl;
    m_location->print(depth+2);
}
    
bool IrFieldDecl::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!m_location->analyze(ctx))
        valid = false;
    
    ctx->popParent();
    
    return valid;
}

bool IrFieldDecl::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    SVariableSymbol symbol;
    if (ctx->lookup(m_location.get(), symbol))
    {
        IrTacStmt tac;
        tac.m_opcode = IrOpcode::GLOBAL;
        tac.m_src0 = m_location->getIdentifier();
        tac.m_info = symbol.m_count * 8;
        ctx->append(tac);
    }
    
    ctx->popParent();
    
    return valid;
}



} // namespace Decaf
