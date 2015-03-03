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
#include "IrMethodCall.h"
#include "IrIdentifier.h"
#include "IrStringLiteral.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrMethodCall::IrMethodCall(int lineNumber, int columnNumber, const std::string& filename, IrStringLiteral* name, IrType type) :
    IrExpression(lineNumber, columnNumber, filename, type),
    m_identifier(nullptr),
    m_externFuncName(name),
    m_externalFunction(true),
    m_arguments()
{
    // strip quotes from literal
    std::string funcName = name->getValue().substr(1, name->getValue().size()-2);   
    m_identifier = new IrIdentifier(lineNumber, columnNumber, filename, funcName);
}
 
IrMethodCall::~IrMethodCall()
{
    delete m_identifier;
    delete m_externFuncName;
    for (auto it : m_arguments)
    {
        delete it;
    }
}
  
void IrMethodCall::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    m_identifier->propagateTypes(ctx);
    for (auto it : m_arguments)
    {
        it->propagateTypes(ctx);
    }
    
    // Update type
    SMethodSymbol symbol;
    if (ctx->lookup(this, symbol))
    {
        setType(symbol.m_type);
    }    
     
    ctx->popParent();    
}
    
void IrMethodCall::print(unsigned int depth)
{
    IRPRINT_INDENT(depth);
    std::cout << "Method Call(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Type = " << IrTypeToString(m_type) << " External = " << (isExternal() ? "true" : "false") << std::endl;
    
    m_identifier->print(depth+1);
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Arguments: " << std::endl;
    for (auto it : m_arguments)
    {
        it->print(depth+2);
    }  
}

bool IrMethodCall::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!isExternal())
    {
        // Method must be in symbol table
        SMethodSymbol symbol;
        if (!ctx->lookup(this, symbol))
        {
            // Method not defined
            std::stringstream msg;
            msg << "method " << getIdentifier()->getIdentifier() << " not defined."; 
            ctx->error(this, msg.str());
            valid = false;
        }
    }
    else
    {
    }
    
    ctx->popParent();
    
    return valid;
}

bool IrMethodCall::codegen(IrTraversalContext* ctx) 
{ 
    if (isExternal())
    {            
        for (auto it : m_arguments)
        {
            it->codegen(ctx);
            
            IrTacStmt tac;
            tac.m_opcode = IrOpcode::PUSH;
            
            IrStringLiteral* sliteral = dynamic_cast<IrStringLiteral*>(it);
            IrLiteral* literal = dynamic_cast<IrLiteral*>(it);
            if (sliteral)
            {
                tac.m_arg0 = sliteral->getIdentifier();
            }
            else if (literal)
            {
                tac.m_arg0 = it;
            }
            else
            {
                tac.m_arg0 = it->getResultIdentifier();
            }
            ctx->append(tac);
        }
        
        IrTacStmt callStmt;
        callStmt.m_opcode = IrOpcode::CALL;
        callStmt.m_arg0 = m_identifier;
        ctx->append(callStmt);
    }
    
    return true; 
}

} // namespace Decaf
