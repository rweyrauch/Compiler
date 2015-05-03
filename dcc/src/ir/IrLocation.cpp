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
#include "IrLocation.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrSymbolTable.h"
#include "IrTravCtx.h"
#include "IrIntLiteral.h"
#include "IrBlock.h"
#include "IrBooleanExpr.h"
#include "IrIfStmt.h"
#include "IrMethodCall.h"
#include "IrStringLiteral.h"
#include "IrExprStmt.h"

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
        
        // allocate a temporary variable for the result of this expression
        m_result = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateTemporary());
        
        createRuntimeChecks(ctx);  
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
        if (m_rangeChecks)
            m_rangeChecks->allocate(ctx);
        
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
        // Insert codegen for runtime index bounds checks.
        if (!codegenRuntimeChecks(ctx))
            valid = false;
            
        if (!m_index->codegen(ctx)) 
            valid = false;

        if (!m_result->codegen(ctx)) 
            valid = false;
            
        if (usedAsWrite())
        {
            IrTacStmt store;
            store.m_opcode = IrOpcode::STORE;
            store.m_src0.build(m_result.get()); // source of the the store is in the m_result identifier
            store.m_src1.build(m_identifier.get());
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
            IrTacStmt load;
            load.m_opcode = IrOpcode::LOAD;
            load.m_src0.build(m_identifier.get());
            if (m_index->getResult())
            {
                load.m_src1.build(m_index->getResult().get());
            }
            else
            {
                load.m_src1.build(dynamic_cast<IrLiteral*>(m_index.get()));
            }
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
 
void IrLocation::createRuntimeChecks(IrTraversalContext* ctx)
{
    // Range checks...
    // m_index->getResult() < location_size && m_index->getResult() >= 0
    m_rangeChecks = std::shared_ptr<IrBlock>(new IrBlock(__LINE__, 0, __FILE__));
    
    SVariableSymbol symbol;
    ctx->lookup(this, symbol);
    
    IrIntegerLiteral* maxRangeValue = new IrIntegerLiteral(__LINE__, 0, __FILE__, "0");
    maxRangeValue->setValue((int)symbol.m_count);
    IrIntegerLiteral* minRangeValue = new IrIntegerLiteral(__LINE__, 0, __FILE__, "0");
    
    IrBooleanExpression* maxCheck = new IrBooleanExpression(__LINE__, 0, __FILE__, m_index.get(), IrBooleanOperator::Less, maxRangeValue);
    IrBooleanExpression* minCheck = new IrBooleanExpression(__LINE__, 0, __FILE__, m_index.get(), IrBooleanOperator::GreaterEqual, minRangeValue);
    IrBooleanExpression* boundCheck = new IrBooleanExpression(__LINE__, 0, __FILE__, maxCheck, IrBooleanOperator::LogicalAnd, minCheck);
    IrBooleanExpression* notCheck = new IrBooleanExpression(__LINE__, 0, __FILE__, nullptr, IrBooleanOperator::Not, boundCheck);
    IrBlock* failedBlock = new IrBlock(__LINE__, 0, __FILE__);
    IrStringLiteral* printFunc = new IrStringLiteral(__LINE__, 0, __FILE__, "\"printf\"");
    IrStringLiteral* abortFunc = new IrStringLiteral(__LINE__, 0, __FILE__, "\"abort\"");
    IrMethodCall* printError = new IrMethodCall(__LINE__, 0, __FILE__, printFunc, IrType::Integer);
    IrStringLiteral* errorMessage = new IrStringLiteral(__LINE__, 0, __FILE__, "\"Runtime bounds check failure.\"");
    printError->addArgument(errorMessage);
    IrMethodCall* abortStatement = new IrMethodCall(__LINE__, 0, __FILE__, abortFunc, IrType::Integer);
    failedBlock->addStatement(new IrExpressionStatement(__LINE__, 0, __FILE__, printError));
    failedBlock->addStatement(new IrExpressionStatement(__LINE__, 0, __FILE__, abortStatement));
    IrIfStatement* check = new IrIfStatement(__LINE__, 0, __FILE__, notCheck, failedBlock);
    
    m_rangeChecks->addStatement(check);
    
    m_rangeChecks.get()->analyze(ctx);
    
}

bool IrLocation::codegenRuntimeChecks(IrTraversalContext* ctx)
{
    bool valid = true;
    if (m_rangeChecks)
    {
        valid = m_rangeChecks->codegen(ctx);
    }
    return valid;
}
 
} // namespace Decaf
