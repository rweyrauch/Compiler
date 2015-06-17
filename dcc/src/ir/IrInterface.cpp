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
#include "IrInterface.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrInterface::~IrInterface()
{
}
    
void IrInterface::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);
    
    m_identifier->propagateTypes(ctx);
    for (auto it : m_method_decl_list)
    {
        it->propagateTypes(ctx);
    }
    
    ctx->popParent();
}
    
void IrInterface::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    
    std::cout << "Interface(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    m_identifier->print(depth+1);
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Method Declarations: " << std::endl;
    for (auto it : m_method_decl_list)
    {
        it->print(depth+2);
    }
}

bool IrInterface::analyze(IrTraversalContext* ctx) 
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    for (auto it : m_method_decl_list)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}

bool IrInterface::codegen(IrTraversalContext* ctx) 
{ 
    ctx->pushParent(this);
    
    m_identifier->codegen(ctx);
    for (auto it : m_method_decl_list)
    {
        it->codegen(ctx);
    }
    
    ctx->popParent();
    
    return true; 
}

void IrInterface::addMethodDecl(IrMethodDeclPtr method)
{
    m_method_decl_list.push_back(method);
}

void IrInterface::addMethodDecl(const std::vector<IrMethodDecl*>& methods)
{
    for (auto it : methods)
    {
        addMethodDecl(IrMethodDeclPtr(it));
    }
}

} // namespace Decaf
