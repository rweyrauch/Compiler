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
#include "IrAssignExpr.h"
#include "IrTravCtx.h"
#include "IrLocation.h"
#include "IrLiteral.h"
#include "IrIdentifier.h"

namespace Decaf
{
static const IrOpcode g_assignOpcodes[(int)IrAssignmentOperator::NUM_IR_ASSIGNMENT_OPERATORS] =
{
    IrOpcode::MOV,
    IrOpcode::ADD,
    IrOpcode::SUB,
 };

static IrOpcode opcodeFor(IrAssignmentOperator assop)
{
    return g_assignOpcodes[(int)assop];
}

void IrAssignExpression::propagateTypes(IrTraversalContext* ctx)
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

void IrAssignExpression::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "Assignment Expr(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    IRPRINT_INDENT(depth+1);
    std::cout << "Type = " << IrTypeToString(m_type) << std::endl;
    
    if (m_lhs)
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "LHS: " << std::endl;
        m_lhs->print(depth+2);
    }
    
    IRPRINT_INDENT(depth+1);
    std::cout << "Operator: " << IrAssignmentOperatorToString(m_operator) << std::endl;
    
    if (m_rhs) 
    {
        IRPRINT_INDENT(depth+1);
        std::cout << "RHS: " << std::endl;
        m_rhs->print(depth+2);
    }
}

bool IrAssignExpression::analyze(IrTraversalContext* ctx)
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
    
    // Rule: lhs must be assignable (a location)
    if (m_lhs)
    {
        if (nullptr == dynamic_cast<IrLocation*>(m_lhs))
        {
            ctx->error(this, "lhs of assigment operator must be assignable.");
            valid = false;
        }
    }
       
    if (m_lhs && m_rhs)
    {
        // Rule: both sides of assignment must have the same type.
        if (m_operator == IrAssignmentOperator::Assign)
        {
            if (m_lhs->getType() != m_rhs->getType())
            {
                ctx->error(this, "lhs and rhs of assigment operator must be of the same type.");               
                valid = false;
            }
            if (m_lhs->isArray())
            {
                ctx->error(this, "lhs of assignment operator may not be an array name.");               
                valid = false;            
            }
            if (m_rhs->isArray())
            {
                ctx->error(this, "rhs of assignment operator may not be an array name.");                
                valid = false;            
            }
        }
        else 
        {
            // Rule: both side of increment/decrement assignment must be integer.
            if (m_lhs->getType() != IrType::Integer)
            {
                ctx->error(this, "lhs of increment/decrement assigment operator must be of type integer.");
                valid = false;
            }
            if (m_rhs->getType() != IrType::Integer)
            {
                ctx->error(this, "rhs of increment/decrement assigment operator must be of type integer.");              
                valid = false;
            }
            if (m_lhs->isArray())
            {
                ctx->error(this, "lhs of increment/decrement assigment operator may not be an array name.");              
                valid = false;            
            }
            if (m_rhs->isArray())
            {
                ctx->error(this, "rhs of increment/decrement assigment operator may not be an array name.");                
                valid = false;            
            }
        }
    }
    
    ctx->popParent();
    
    return valid;
}

bool IrAssignExpression::codegen(IrTraversalContext* ctx)
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
        m_tac.m_opcode = opcodeFor(m_operator);
        m_tac.m_arg0 = nullptr;
        m_tac.m_arg1 = nullptr;
        m_tac.m_arg2 = nullptr;
        if (m_lhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_lhs);
            if (literal)
            {
                m_tac.m_arg0 = m_lhs;
            }
            else
            {
                m_tac.m_arg0 = m_lhs->getResultIdentifier();
            }
        }
        if (m_rhs != nullptr)
        {
            IrLiteral* literal = dynamic_cast<IrLiteral*>(m_rhs);
            if (literal)
            {
                m_tac.m_arg1 = m_rhs;
            }
            else
            {
                m_tac.m_arg1 = m_rhs->getResultIdentifier();
            }
        }       
        m_tac.m_arg2 = getResultIdentifier();
        
        IrPrintTac(m_tac);
    }
    ctx->popParent();
    
    return valid;
}

} // namespace Decaf
