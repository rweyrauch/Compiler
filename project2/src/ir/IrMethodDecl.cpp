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
#include "IrMethodDecl.h"
#include "IrVarDecl.h"
#include "IrBlock.h"
#include "IrTravCtx.h"
#include "IrReturnStmt.h"

namespace Decaf
{

void IrMethodDecl::clean(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
	ctx->pushParent(this);
    
    m_identifier->clean(ctx);
    for (auto it : m_argument_list)
    {
        it->clean(ctx);
    }
    if (m_block) m_block->clean(ctx);
    
    ctx->popParent();
    ctx->popSymbols();
}

void IrMethodDecl::print(unsigned int depth)
{
    IRPRINT_INDENT(depth);
    std::cout << "Method(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Name: " << std::endl;
    m_identifier->print(depth+2);
    
    m_symbols->print(depth+1);
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Return Type: " << IrTypeToString(m_returnType) << std::endl;
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Arguments: " << std::endl;
    for (auto it : m_argument_list)
    {
        it->print(depth+2);
    }  
    
    if (m_block) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "Body: " << std::endl;        
        m_block->print(depth+2);
    }
}
    
bool IrMethodDecl::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
 
    ctx->pushSymbols(m_symbols);
	ctx->pushParent(this);
    
    if (m_block) 
    {
        valid = m_block->analyze(ctx);
    
        // Check return statement(s) types.    
        const size_t numStmts = m_block->getNumStatements();
        for (size_t i = 0; i < numStmts; i++)
        {
            const IrReturnStatement* retStmt = dynamic_cast<const IrReturnStatement*>(m_block->getStatement(i));
            if (retStmt)
            {
                if (retStmt->getReturnType() != m_returnType)
                {
                    // Error - incorrect return type.
                    std::cerr << getFilename() << ":" << getLineNumber() << ":" << getColumnNumber() <<  ": error: incorrect return type.  Expected " << IrTypeToString(m_returnType)
                              << " but found " << IrTypeToString(retStmt->getReturnType()) << std::endl;
                    valid = false;
                }
            }
        }
    }
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}
    
} // namespace Decaf
