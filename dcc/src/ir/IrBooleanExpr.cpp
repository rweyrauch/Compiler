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
#include "IrBooleanExpr.h"
#include "IrLiteral.h"
#include "IrIdentifier.h"
#include "IrTravCtx.h"

namespace Decaf
{
static const IrOpcode g_booleanOpcodes[(int)IrBooleanOperator::NUM_IR_BOOLEAN_OPERATORS] =
{
    IrOpcode::EQUAL,
    IrOpcode::NOTEQUAL,
    IrOpcode::LESS, 
    IrOpcode::LESSEQUAL,
    IrOpcode::GREATER,
    IrOpcode::GREATEREQUAL,
    IrOpcode::AND,
    IrOpcode::OR,
    IrOpcode::NOT,
};

static IrOpcode opcodeFor(IrBooleanOperator boolop)
{
    return g_booleanOpcodes[(int)boolop];
}

void IrBooleanExpression::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    if (m_lhs) m_lhs->propagateTypes(ctx);
    if (m_rhs) m_rhs->propagateTypes(ctx);
    
    if (getType() == IrType::Unknown)
    {
        setType(IrType::Boolean);
    }   
    
    ctx->popParent();    
}

void IrBooleanExpression::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Boolean Expr(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Type = " << IrTypeToString(m_type) << std::endl;

    if (m_lhs)
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "LHS: " << std::endl;
        m_lhs->print(depth+2);
    }
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Operator: " << IrBooleanOperatorToString(m_operator) << std::endl;
    
    if (m_rhs) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "RHS: " << std::endl;
        m_rhs->print(depth+2);
    }
}

bool IrBooleanExpression::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
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
    
    if (m_operator == IrBooleanOperator::Not)
    {
        // Rule: Not operator has no lhs.
        if (m_lhs != nullptr)
        {
            valid = false;
        }
        
        // Rule: RHS of Not operator must be of type boolean.
        if (m_rhs != nullptr)
        {
            if (m_rhs->getType() != IrType::Boolean)
            {
                ctx->error(this, "rhs of not operator(!) must of type boolean.");
                valid = false;             
            }
            if (m_rhs->isArray())
            {
                ctx->error(this, "rhs of not operator(!) may not be an array name.");
                valid = false;            
            }
        }
        else
        {
            ctx->error(this, "not operator(!) requires a rhs expression.");
            valid = false;
        }
    }
    else
    {
        // Rule: both sides of the comparison must be of type integer.
        if (m_lhs)
        {
            if (m_lhs->getType() != IrType::Integer)
            {
                ctx->error(this, "lhs of boolean expression must be of type integer.");
                valid = false;
            }
            if (m_lhs->isArray())
            {
                ctx->error(this, "lhs of boolean expression may not be an array name.");
                valid = false;            
            }
        }
        
        if (m_rhs)
        {
            if (m_rhs->getType() != IrType::Integer)
            {
                ctx->error(this, "rhs of boolean expression must be of type integer.");
                valid = false;
            }
            if (m_rhs->isArray())
            {
                ctx->error(this, "rhs of boolean expression may not be an array name.");
                valid = false;            
            }
        }
    }
    ctx->popParent();
    
    return valid;
}

bool IrBooleanExpression::codegen(IrTraversalContext* ctx)
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
    
    if (valid)
    {
        // allocate a temporary variable for the result of this expression
        m_result = IrIdentifier::CreateTemporary();
        if (!ctx->addTempVariable(m_result, m_type))
        {
            ctx->error(m_result, "Internal compiler error.  Failed to add temporary variable to symbol table.");
            valid = false;
        }
        
        // TAC:
        // tempResult = lhs operator rhs
        IrTacStmt tac;
        tac.m_opcode = opcodeFor(m_operator);
        if (m_lhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_lhs);
            if (literal)
            {
                tac.m_arg0 = m_lhs;
            }
            else
            {
                tac.m_arg0 = m_lhs->getResultIdentifier();
            }
        }
        if (m_rhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_rhs);
            if (literal)
            {
                tac.m_arg1 = m_rhs;
            }
            else
            {
                tac.m_arg1 = m_rhs->getResultIdentifier();
            }
        }       
        tac.m_arg2 = getResultIdentifier();
        
        ctx->append(tac);
    }
    ctx->popParent();
    
    return valid;
}

} // namespace Decaf
