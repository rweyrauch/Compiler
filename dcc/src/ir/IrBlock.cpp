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
#include "IrForStmt.h"

namespace Decaf
{

 IrBlock::~IrBlock()
{
    for (auto it : m_variables)
        delete it;
    for (auto it : m_statements)
        delete it;
    delete m_symbols;
}

void IrBlock::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
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
    
    ctx->pushSymbols(m_symbols);
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
    
bool IrBlock::codegen(IrTraversalContext* ctx) 
{
    bool valid = true;
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    for (auto it : m_variables)
    {
        if (!it->codegen(ctx)) valid = false;
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
    m_variables.push_back(var);
    
    m_symbols->addVariable(var);
}

void IrBlock::addVariableDecl(const std::vector<IrVariableDecl*>& variables)
{
    m_variables.insert(m_variables.end(), variables.begin(), variables.end());
    
    for (auto it : m_variables)
    {
        m_symbols->addVariable(it);
    }
}
 
size_t IrBlock::getAllocationSize() const
{
	size_t allocSize = m_symbols->getAllocationSize();
	
	// recurse sub-blocks
	for (auto it = m_statements.cbegin(); it != m_statements.end(); ++it)
	{
		IrBlock const* subblock = dynamic_cast<IrBlock const*>(*it);
		IrForStatement const* forblock = dynamic_cast<IrForStatement const*>(*it);
		if (subblock != nullptr)
		{
			allocSize += subblock->getAllocationSize();
		}
		if (forblock != nullptr)
		{
			allocSize += forblock->getAllocationSize();
		}
	}
	return allocSize;
}
	    
} // namespace Decaf
