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
#include "IrMethodDecl.h"
#include "IrIntLiteral.h"
#include "IrTravCtx.h"
#include "IrReturnStmt.h"

namespace Decaf
{

IrMethodDecl::~IrMethodDecl()
{
}
 
void IrMethodDecl::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    m_identifier->propagateTypes(ctx);
    for (auto it : m_argument_list)
    {
        it->propagateTypes(ctx);
    }
    if (m_block) m_block->propagateTypes(ctx);
    
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
 
    ctx->addString(IrIdentifier::CreateLabel().get(), getName());
    
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    if (m_block) 
    {
        valid = m_block->analyze(ctx);
    
        bool returnFound = false;
        // Check return statement(s) types.    
        const size_t numStmts = m_block->getNumStatements();
        for (size_t i = 0; i < numStmts; i++)
        {
            const IrReturnStatement* retStmt = dynamic_cast<const IrReturnStatement*>(m_block->getStatement(i).get());
            if (retStmt)
            {
                if (retStmt->getReturnType() != m_returnType)
                {
                    // Error - incorrect return type.
                    std::stringstream msg;                    
                    msg << "incorrect return type.  Expected " << IrTypeToString(m_returnType) << " but found " << IrTypeToString(retStmt->getReturnType()) << ".";
                    ctx->error(this, msg.str());
                    valid = false;
                }
                returnFound = true;
            }
        }
        
        if (!returnFound)
        {
            if (m_returnType != IrType::Void)
            {
                ctx->error(this, "method must have a return statement.");
                valid = false;
            }
            else
            {
                // add the implied return statement
                m_block->addStatement(IrStatementPtr(new IrReturnStatement(m_block->getLineNumber(), m_block->getColumnNumber(), m_block->getFilename())));
            }
        }
        // patch up child symbol addresses
        m_block->setSymbolStartAddress(m_symbols->getAllocationSize());
    }
       
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}
    
bool IrMethodDecl::allocate(IrTraversalContext* ctx) 
{ 
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);

    m_identifier->allocate(ctx);
    
    // determine the stack storage requirements for the argument and body
    m_stackSize = m_symbols->getAllocationSize();
    if (m_block != nullptr) 
        m_stackSize += m_block->getAllocationSize();
    // round stack size to multiple of 16 (assuming already a multiple of 8)
    if (m_stackSize % 16 != 0)
        m_stackSize += 8;
     
    for (auto it : m_argument_list)
    {
        it->allocate(ctx);
    }
    
    if (m_block) m_block->allocate(ctx);
   
    ctx->popParent();
    ctx->popSymbols();
    
    return true; 
}
    
bool IrMethodDecl::codegen(IrTraversalContext* ctx) 
{ 
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);

    m_identifier->codegen(ctx);
    
    IrTacStmt beginTac(IrOpcode::FBEGIN, getLineNumber());
    beginTac.m_src0.build(m_identifier.get());
    beginTac.m_info = (int)m_stackSize;
    
    ctx->append(beginTac);
    
    int argNum = 0;
    for (auto it : m_argument_list)
    {
        it->codegen(ctx);
        
        IrTacStmt paramTac(IrOpcode::GETPARAM, it->getLineNumber());
        paramTac.m_src0.build(it->getVariable(0).get());
        paramTac.m_info = argNum;
        argNum++;

        ctx->append(paramTac);
    }
    
    if (m_block) m_block->codegen(ctx);
   
    ctx->popParent();
    ctx->popSymbols();
    
    return true; 
}
    
const std::string& IrMethodDecl::asString() const
{
    return m_identifier->asString();
}
    
} // namespace Decaf
