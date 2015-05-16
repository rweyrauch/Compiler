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
#include "IrBlock.h"
#include "IrVarDecl.h"
#include "IrStatement.h"
#include "IrSymbolTable.h"
#include "IrTravCtx.h"
#include "IrIntLiteral.h"
#include "IrBoolLiteral.h"
#include "IrDoubleLiteral.h"

namespace Decaf
{

 IrBlock::~IrBlock()
{
}

void IrBlock::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_variables)
    {
        it->propagateTypes(ctx);
    }
    for (auto it : m_statements)
    {
        it->propagateTypes(ctx);
    }
    
    ctx->popParent();
    ctx->popSymbols();
}

void IrBlock::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Block(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    m_symbols->print(depth+1);
    
    for (auto it : m_variables)
    {
        it->print(depth+1);
    }
    for (auto it : m_statements)
    {
        it->print(depth+1);
    }
}
    
bool IrBlock::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_variables)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    for (auto it : m_statements)
    {
        if (!it->analyze(ctx))
            valid = false;
    }   
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

bool IrBlock::allocate(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_variables)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    for (auto it : m_statements)
    {
        if (!it->allocate(ctx))
            valid = false;
    }   
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

bool IrBlock::codegen(IrTraversalContext* ctx) 
{
    bool valid = true;
    ctx->pushSymbols(m_symbols.get());
    ctx->pushParent(this);
    
    for (auto it : m_variables)
    {
        if (!it->codegen(ctx)) valid = false;
    }
    
    // initialize all variables
    for (auto it : m_variables)
    {
        size_t numVars = it->getNumVariables();
        for (size_t i = 0; i < numVars; i++)
        {
            auto var = it->getVariable(i);
            IrTacStmt initVar(IrOpcode::MOV, var->getLineNumber());
            if (it->getType() == IrType::Integer) {
                initVar.m_src0.build(IrIntegerLiteral::GetZero().get());
            }
            else if (it->getType() == IrType::Boolean)
            {
                initVar.m_src0.build(IrBooleanLiteral::GetFalse().get());
            }
            else if (it->getType() == IrType::Double)
            {
                initVar.m_src0.build(IrDoubleLiteral::GetZero().get());
            }
            initVar.m_dst.build(var.get());
            
            ctx->append(initVar);
        }
    }
    
    for (auto it : m_statements)
    {
        if (!it->codegen(ctx)) valid = false;
    }
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid; 
}
    
void IrBlock::addVariableDecl(IrVariableDecl* var)
{
    m_variables.push_back(std::shared_ptr<IrVariableDecl>(var));
    
    m_symbols->addVariable(var);
}

void IrBlock::addVariableDecl(const std::vector<IrVariableDecl*>& variables)
{
    for (auto it : variables)
    {
        addVariableDecl(it);
    }
}
 
size_t IrBlock::getAllocationSize() const
{
    size_t allocSize = m_symbols->getAllocationSize();

    // recurse sub-blocks
    for (auto it : m_statements)
    {
        allocSize += it->getAllocationSize();
    }
    return allocSize;
}
    
void IrBlock::setSymbolStartAddress(size_t addr)
{
    m_symbols->setStartAddress(addr);

    // recurse sub-blocks
    for (auto it : m_statements)
    {
        it->setSymbolStartAddress(m_symbols->getAllocationSize() + addr);
    }
}
    
} // namespace Decaf
