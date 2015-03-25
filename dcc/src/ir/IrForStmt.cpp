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
#include "IrForStmt.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrBooleanExpr.h"
#include "IrBinaryExpr.h"
#include "IrIdentifier.h"
#include "IrAssignExpr.h"
#include "IrIntLiteral.h"
#include "IrBlock.h"
#include "IrGotoStmt.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrForStatement::IrForStatement(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* loopVar, IrExpression* initialExpr, IrExpression* endExpr, IrBlock* block) :
    IrStatement(lineNumber, columnNumber, filename),
    m_loopVar(nullptr),
    m_initialValue(std::shared_ptr<IrExpression>(initialExpr)),
    m_terminatingValue(std::shared_ptr<IrExpression>(endExpr)),
    m_body(std::shared_ptr<IrBlock>(block))
{
    m_loopVar = std::shared_ptr<IrLocation>(new IrLocation(lineNumber, columnNumber, filename, loopVar, IrType::Integer));

    m_initLoopAuto = std::shared_ptr<IrExpression>(new IrAssignExpression(lineNumber, columnNumber, filename, m_loopVar.get(), IrAssignmentOperator::Assign, m_initialValue.get()));
    m_labelTop = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());
    m_labelContinue = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());
    m_labelEnd = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());
    
    m_terminatingExpr = std::shared_ptr<IrExpression>(new IrBooleanExpression(lineNumber, columnNumber, filename, m_loopVar.get(), IrBooleanOperator::Less, m_terminatingValue.get()));

    m_loopIncrement = std::shared_ptr<IrIntegerLiteral>(new IrIntegerLiteral(lineNumber, columnNumber, filename, "1"));
    m_incrementLoop = std::shared_ptr<IrExpression>(new IrAssignExpression(lineNumber, columnNumber, filename, m_loopVar.get(), IrAssignmentOperator::IncrementAssign, m_loopIncrement.get()));

    m_loopGoto = std::shared_ptr<IrGotoStatement>(new IrGotoStatement(lineNumber, columnNumber, filename, m_labelTop.get()));
}
    
IrForStatement::~IrForStatement()
{
}
    
void IrForStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);
    
    m_loopVar->propagateTypes(ctx);    
    m_initialValue->propagateTypes(ctx);
    m_terminatingValue->propagateTypes(ctx);
    
    if (m_body) m_body->propagateTypes(ctx);
    
    m_labelTop->propagateTypes(ctx);
    m_labelContinue->propagateTypes(ctx);
    m_labelEnd->propagateTypes(ctx);
    m_initLoopAuto->propagateTypes(ctx);
    m_terminatingExpr->propagateTypes(ctx);
    m_loopIncrement->propagateTypes(ctx);
    m_incrementLoop->propagateTypes(ctx);
    m_loopGoto->propagateTypes(ctx);
    
    ctx->popParent();
}
    
void IrForStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "For(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
           
    m_loopVar->print(depth+1);
    m_initialValue->print(depth+1);
    m_terminatingValue->print(depth+1);
    
    if (m_body) m_body->print(depth+1);
}

bool IrForStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!m_loopVar->analyze(ctx))
        valid = false;
    if (!m_initialValue->analyze(ctx))
        valid = false;
    if (!m_terminatingValue->analyze(ctx))
        valid = false;
        
    if (m_body) 
    {
        if (!m_body->analyze(ctx))
            valid = false;
    }
    
    // Initial variable and terminal value expressions must be of type integer.
    if (m_initialValue->getType() != IrType::Integer)
    {
        std::stringstream msg;
        msg << "for loop initial value expression must be of type integer.  Got: " << IrTypeToString(m_initialValue->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }
    if (m_terminatingValue->getType() != IrType::Integer)
    {
        std::stringstream msg;
        msg << "for loop ending value expression must be of type integer.  Got: " << IrTypeToString(m_terminatingValue->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }

    m_labelTop->analyze(ctx);
    m_labelContinue->analyze(ctx);
    m_labelEnd->analyze(ctx);
    m_initLoopAuto->analyze(ctx);
    m_terminatingExpr->analyze(ctx);
    m_loopIncrement->analyze(ctx);
    m_incrementLoop->analyze(ctx);
    m_loopGoto->analyze(ctx);
        
    ctx->popParent();
    
    return valid;
}

bool IrForStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    // int loop_var = <initial value>
    // LABEL_TOP:
    // if (loop_var < term_var)
    // {
    //     <body>
    // LABEL_CONTINUE:
    //     loop_var += 1;
    //     goto LABEL_TOP;
    // }
    // LABEL_END:
    //
    
    ctx->pushParent(this);
    
    m_initLoopAuto->codegen(ctx);
    m_labelTop->codegen(ctx);
    
    IrTacStmt label;
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelTop;
    ctx->append(label);
     
    m_terminatingExpr->codegen(ctx);
    
    IrTacStmt tac;
    tac.m_opcode = IrOpcode::IFZ;
    tac.m_arg0 = m_terminatingExpr->getResultIdentifier();
    tac.m_arg1 = m_labelEnd;
    tac.m_arg2 = nullptr;
    
    ctx->append(tac);
      
    if (m_body) m_body->codegen(ctx);
    m_labelContinue->codegen(ctx);
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelContinue;
    ctx->append(label);
    
    m_incrementLoop->codegen(ctx);
    m_loopGoto->codegen(ctx);
    
    m_labelEnd->codegen(ctx);
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelEnd;
    ctx->append(label);
     
    ctx->popParent();
    
    return valid;
}

size_t IrForStatement::getAllocationSize() const
{
    size_t allocSize = 0;
    if (m_body != nullptr)
    {
        allocSize += m_body->getAllocationSize();
    }
    return allocSize;
}

void IrForStatement::setSymbolStartAddress(size_t addr)
{
    if (m_body != nullptr)
    {
        m_body->setSymbolStartAddress(addr);
    }
}
 
} // namespace Decaf
