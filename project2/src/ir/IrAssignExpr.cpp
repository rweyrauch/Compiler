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

namespace Decaf
{

void IrAssignExpression::clean(IrTraversalContext* ctx)
{
    ctx->pushParent(this);
    
    if (m_lhs) m_lhs->clean(ctx);
    if (m_rhs) m_rhs->clean(ctx);
    
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
    
    if (m_lhs && m_rhs)
    {
        if (m_lhs->getType() != m_rhs->getType())
        {
            std::cerr << getFilename() << ":" << getLineNumber() << ":" << getColumnNumber() << ": error: lhs and rhs of assigment operator must be of the same type." << std::endl;
            
            valid = false;
        }
    }
    
    ctx->popParent();
    
    return valid;
}

} // namespace Decaf
