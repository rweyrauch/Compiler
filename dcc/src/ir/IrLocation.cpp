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

namespace Decaf
{

int IrLocation::s_tempLocationCounter = 0;

IrLocation* IrLocation::CreateTemporary(IrType type)
{
    std::stringstream tempName;
    tempName << ".LC" << s_tempLocationCounter++;
    IrIdentifier* tempId = new IrIdentifier(0, 0, __FILE__, tempName.str());
    IrLocation* tempLoc = new IrLocation(0, 0, __FILE__, tempId, type);
    return tempLoc;
}
    
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
    std::cout << "Type: " << IrTypeToString(m_type) << " Array: " << (isArray() ? "true" : "false") << std::endl;
    
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
    }
       
    ctx->popParent();
       
    return valid;
}

bool IrLocation::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    ctx->pushParent(this);
    
    if (!m_identifier->codegen(ctx)) valid = false;
    if (m_index) 
    {
        if (!m_index->codegen(ctx)) valid = false;
       
        // TODO: move index result into %rsi register!!!
        IrTacStmt tac;
        tac.m_opcode = IrOpcode::MOV;
        tac.m_arg0 = nullptr; // m_index->getResult()->getIdentifier();
        tac.m_arg2 = nullptr; // %rsi
        ctx->append(tac);
    }
    
    if (!usedAsDeclaration())
    {
        m_result = this;
    }
    
    ctx->popParent();
    
    return valid;
}
 
} // namespace Decaf
