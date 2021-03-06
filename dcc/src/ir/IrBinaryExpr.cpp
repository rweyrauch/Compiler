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
#include <cassert>
#include "IrCommon.h"
#include "IrBinaryExpr.h"
#include "IrLiteral.h"
#include "IrLocation.h"
#include "IrTravCtx.h"

namespace Decaf
{
static const IrOpcode g_binaryOpcodes[(int)IrBinaryOperator::NUM_IR_BINARY_OPERATORS] =
{
    IrOpcode::ADD,
    IrOpcode::SUB,
    IrOpcode::MUL,
    IrOpcode::DIV,
    IrOpcode::MOD,
};

static IrOpcode opcodeFor(IrBinaryOperator binop)
{
    return g_binaryOpcodes[(int)binop];
}

void IrBinaryExpression::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    if (m_lhs) m_lhs->propagateTypes(ctx);
    if (m_rhs) m_rhs->propagateTypes(ctx);
    
    if (getType() == IrType::Unknown)
    {
        if (m_lhs)
            setType(m_lhs->getType());
        else if (m_rhs)
            setType(m_rhs->getType());
    }    
    
    ctx->popParent();    
}
    
void IrBinaryExpression::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Binary Expr(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Type = " << IrTypeToString(m_type) << std::endl;
    
    if (m_lhs)
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "LHS: " << std::endl;
        m_lhs->print(depth+2);
    }
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Operator: " << IrBinaryOperatorToString(m_operator) << std::endl;
    
    if (m_rhs) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "RHS: " << std::endl;
        m_rhs->print(depth+2);
    }
}

bool IrBinaryExpression::analyze(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    bool valid = true;
    
    if (m_lhs) 
    {
        if (!m_lhs->analyze(ctx))
            valid = false;
    }
    if (m_rhs) 
    {
        if (!m_rhs->analyze(ctx))
            valid = false;
    }
    
    if (m_lhs)
    {
        if (!IsNumeric(m_lhs->getType()))
        {
            ctx->error(this, "lhs of binary expression must be of type integer or double.");           
            valid = false;
        }
        if (m_lhs->isArray())
        {
            ctx->error(this, "lhs of binary expression may not be an array name.");            
            valid = false;            
        }
    }
    
    if (m_rhs)
    {
        if (!IsNumeric(m_rhs->getType()))
        {
            ctx->error(this, "rhs of binary expression must be of type integer or double.");          
            valid = false;
        }
        if (m_rhs->isArray())
        {
            ctx->error(this, "rhs of binary expression may not be an array name.");
            valid = false;            
        }
    }
    
    if (valid)
    {
        if (m_rhs && m_lhs)
        {
            if (m_rhs->getType() != m_lhs->getType())
            {
                ctx->error(this, "lhs and rhs of binary expression must have the same type.");
                valid = false;
            }
        }
    }
    
    if (valid)
    {
        assert(m_result == nullptr);
        // allocate a temporary variable for the result of this expression
        m_result = IrIdentifier::CreateTemporary();
    }   
    
    ctx->popParent();
    
    return valid;
}

bool IrBinaryExpression::allocate(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    bool valid = true;
    
    if (m_lhs) 
    {
        if (!m_lhs->allocate(ctx))
            valid = false;
    }
    if (m_rhs) 
    {
        if (!m_rhs->allocate(ctx))
            valid = false;
    }

    if (valid)
    {
        // allocate a temporary variable for the result of this expression
        if (!ctx->addTempVariable(m_result.get(), m_type))
        {
            ctx->error(m_result.get(), "Internal compiler error.  Failed to add temporary variable to symbol table.");
            valid = false;
        }
    }   
    
    ctx->popParent();
    
    return valid;
}

bool IrBinaryExpression::codegen(IrTraversalContext* ctx)
{ 
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (m_lhs) 
    {
        valid = m_lhs->codegen(ctx);
    }
    if (m_rhs) 
    {
        valid = m_rhs->codegen(ctx);
    }
    
    if (m_result != nullptr)
        if (!m_result->codegen(ctx))
            valid = false;
        
    if (valid)
    {
        // TAC:
        // tempResult = lhs operator rhs
        IrTacStmt tac(opcodeFor(m_operator), getLineNumber());
        if (m_lhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_lhs.get());
            if (literal)
            {
                tac.m_src0.build(literal);
            }
            else
            {
                 tac.m_src0.build(m_lhs->getResult().get());
            }
        }
        if (m_rhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_rhs.get());
            if (literal)
            {
                tac.m_src1.build(literal);
            }
            else
            {
                tac.m_src1.build(m_rhs->getResult().get());
            }
        }
       
        tac.m_dst.build(getResult().get());
        
        ctx->append(tac);
    }
    ctx->popParent();
    
    return valid;
}

const std::string& IrBinaryExpression::asString() const
{
    return IrBinaryOperatorToString(m_operator);
}

} // namespace Decaf
