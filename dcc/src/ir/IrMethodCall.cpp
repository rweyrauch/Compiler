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
#include "IrLocation.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrMethodCall::IrMethodCall(int lineNumber, int columnNumber, const std::string& filename, IrStringLiteral* name, IrType type) :
    IrExpression(lineNumber, columnNumber, filename, type),
    m_identifier(nullptr),
    m_externFuncName(std::shared_ptr<IrStringLiteral>(name)),
    m_externalFunction(true),
    m_arguments()
{
    // strip quotes from literal
    std::string funcName = name->getValue().substr(1, name->getValue().size()-2);   
    m_identifier = std::shared_ptr<IrIdentifier>(new IrIdentifier(lineNumber, columnNumber, filename, funcName));
}
 
IrMethodCall::~IrMethodCall()
{
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
    
    if (!m_identifier->analyze(ctx))
        valid = false;
    
    for (auto it : m_arguments)
    {
        if (!it->analyze(ctx))
            valid = false;
    }
    
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
        // External method identifier must not be empty string.
        if (getIdentifier()->getIdentifier().empty())
        {
            std::stringstream msg;
            msg << "invalid external method name.";
            ctx->error(this, msg.str());
            valid = false;
        }
    }
   
    ctx->popParent();
    
    return valid;
}

bool IrMethodCall::allocate(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!m_identifier->allocate(ctx))
        valid = false;
    
    for (auto it : m_arguments)
    {
        if (!it->allocate(ctx))
            valid = false;
    }
    
    if (getType() != IrType::Void)
    {
        m_result = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateTemporary());      
        ctx->addTempVariable(m_result.get(), getType());
    }
    
    ctx->popParent();
    
    return valid;
}

bool IrMethodCall::codegen(IrTraversalContext* ctx) 
{ 
    bool valid = true;
    
    ctx->pushParent(this);
    
    for (auto it : m_arguments)
    {
        if (!it->codegen(ctx))
            valid = false;
    }
    
    int argCount = 0;    
    for (auto it : m_arguments)
    {    
        IrTacStmt tac;
        tac.m_opcode = IrOpcode::PARAM;
        
        IrStringLiteral* sliteral = dynamic_cast<IrStringLiteral*>(it.get());
        IrLiteral* literal = dynamic_cast<IrLiteral*>(it.get());
        if (sliteral)
        {
            tac.m_src0.build(sliteral->getIdentifier().get());
        }
        else if (literal)
        {
             tac.m_src0.build(literal);
        }
        else
        {
            tac.m_src0.build(it->getResult().get());
        }
        tac.m_info = argCount++;
        ctx->append(tac);
    }
    
    if (m_result != nullptr)
        if (!m_result->codegen(ctx))
            valid = false;
    
    IrTacStmt callStmt;
    callStmt.m_opcode = IrOpcode::CALL;
    callStmt.m_src0.build(m_identifier.get());
    callStmt.m_src1.build(m_result.get());
    ctx->append(callStmt);
    
    ctx->popParent();
    
    return valid; 
}

const std::string& IrMethodCall::asString() const
{
    return m_identifier->asString();
}

} // namespace Decaf
