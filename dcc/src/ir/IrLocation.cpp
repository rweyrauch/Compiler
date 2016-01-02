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
#include <cassert>
#include "IrCommon.h"
#include "IrLocation.h"
#include "IrSymbolTable.h"
#include "IrTravCtx.h"
#include "IrIntLiteral.h"
#include "IrIfStmt.h"
#include "IrMethodCall.h"
#include "IrStringLiteral.h"

namespace Decaf
{
 
void IrLocation::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    if (m_index) m_index->propagateTypes(ctx);
    
    // Update type
    SVariableSymbol symbol;
    if (ctx->lookup(this, symbol))
    {
        bool isArray = false;
        if (usedAsDeclaration() && (m_index != nullptr))
        {
            isArray = true;
        }
        else // variable
        {
            isArray = (m_index == nullptr) && (symbol.m_count > 1);
        }
        setType(symbol.m_type, isArray);
    }
    
    ctx->popParent();
}
    
void IrLocation::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Location(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Type: " << IrTypeToString(m_type) << " Array: " << (isArray() ? "true" : "false")
              << " Store: " << (usedAsWrite() ? "true" : "false") << std::endl;
    
    if (m_identifier) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "Identifier: " << std::endl;
        m_identifier->print(depth+2);
    }
    if (m_index) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "Index: " << std::endl;
        m_index->print(depth+2);
    }
}

bool IrLocation::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    bool symbolValid = false;
    
    ctx->pushParent(this);
    
    // Location must be in symbol table
    SVariableSymbol symbol;
    if (!ctx->lookup(this, symbol))
    {
        // Variable not defined
        std::stringstream msg;
        msg  << "variable \'" << getIdentifier()->getIdentifier() << "\' not declared."; 
        ctx->error(this, msg.str());
        valid = false;
    }
    else
    {
        symbolValid = true;
    }
   
    if (m_index)
    {
        valid = m_index->analyze(ctx);

        // Location index must be an integer expression
        if (m_index->getType() != IrType::Integer)
        {
            // Error - index not an integer
            std::stringstream msg;
            msg << "array \'" << getIdentifier()->getIdentifier() << "\' index must be an integer expression.  Got: " << IrTypeToString(m_index->getType()) << "."; 
            ctx->error(this, msg.str());
            valid = false;
        }
        else
        {
            // Location index must be in range of symbol count when not declaring an array.
            IrIntegerLiteral* intLit = dynamic_cast<IrIntegerLiteral*>(m_index.get());
            if (intLit && !usedAsDeclaration() && symbolValid)
            {
                if (intLit->getValue() < 0 || (size_t)intLit->getValue() >= symbol.m_count)
                {
                    // Error - index out of range
                    std::stringstream msg;                    
                    msg << "array \'" << getIdentifier()->getIdentifier() << "\' index out of range.  Max value: "
                        << symbol.m_count << " but given " << intLit->getValue() << "."; 
                    ctx->error(this, msg.str());
                    valid = false;                    
                }
            }
        }
        
        assert(m_result == nullptr);
        // allocate a temporary variable for the result of this expression
        m_result = IrIdentifier::CreateTemporary();
    }
    else
    {
        m_result = m_identifier;
    }
    
    ctx->popParent();
       
    return valid;
}

bool IrLocation::allocate(IrTraversalContext* ctx)
{
    bool valid = true;
    ctx->pushParent(this);
    
    if (!m_identifier->allocate(ctx)) 
        valid = false;
    
    if (m_index) 
    {        
        if (!m_index->allocate(ctx)) 
            valid = false;

        if (!ctx->addTempVariable(m_result.get(), m_type))
        {
            ctx->error(m_result.get(), "Internal compiler error.  Failed to add temporary variable to symbol table.");
            valid = false;
        } 
    }
        
    ctx->popParent();
    
    return valid;
}

bool IrLocation::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    ctx->pushParent(this);
    
    if (!m_identifier->codegen(ctx)) 
        valid = false;
    
    if (m_index) 
    {                                 
        if (!m_index->codegen(ctx)) 
            valid = false;
                
        if (!m_result->codegen(ctx)) 
            valid = false;
        
        SVariableSymbol symbol;
        bool found = ctx->lookup(m_identifier.get(), symbol);
        assert(found);
        
        if (usedAsWrite())
        {
            IrTacStmt store(IrOpcode::STORE, getLineNumber());
            store.m_src0.build(m_result.get()); // source of the the store is in the m_result identifier
            store.m_src1.build(m_identifier.get());
            store.m_info = symbol.m_count;
            if (m_index->getResult())
            {
                store.m_dst.build(m_index->getResult().get());
            }
            else
            {
                store.m_dst.build(dynamic_cast<IrLiteral*>(m_index.get()));
            }
            ctx->append(store);
        }
        else
        {
            IrTacStmt load(IrOpcode::LOAD, getLineNumber());
            load.m_src0.build(m_identifier.get());
            if (m_index->getResult())
            {
                load.m_src1.build(m_index->getResult().get());
            }
            else
            {
                load.m_src1.build(dynamic_cast<IrLiteral*>(m_index.get()));
            }
            load.m_info = symbol.m_count;
            load.m_dst.build(m_result.get());
            
            ctx->append(load);
        }
    }
        
    ctx->popParent();
    
    return valid;
}
 
const std::string& IrLocation::asString() const
{
    return m_identifier->asString();
}

} // namespace Decaf
