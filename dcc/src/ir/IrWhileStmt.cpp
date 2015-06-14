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
#include "IrWhileStmt.h"
#include "IrIdentifier.h"
#include "IrExpression.h"
#include "IrBooleanExpr.h"
#include "IrBlock.h"
#include "IrGotoStmt.h"
#include "IrTravCtx.h"

namespace Decaf
{
// Template:
//
// LABEL_TOP:
// if (<terminate expr>)
// {
//     <body>
// LABEL_CONTINUE:
//     goto LABEL_TOP;
// }
// LABEL_END:

IrWhileStatement::IrWhileStatement(int lineNumber, int columnNumber, const std::string& filename, IrExpression* loopExpr, IrStatement* block) :
    IrStatement(lineNumber, columnNumber, filename),
    m_loopExpr(loopExpr),
    m_body(std::shared_ptr<IrStatement>(block))
{
    m_labelTop = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());
    m_labelContinue = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());
    m_labelEnd = std::shared_ptr<IrIdentifier>(IrIdentifier::CreateLabel());

    m_loopGoto = std::shared_ptr<IrGotoStatement>(new IrGotoStatement(lineNumber, columnNumber, filename, m_labelTop.get()));    
}
    
IrWhileStatement::~IrWhileStatement()
{
}
    
void IrWhileStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);
    
    m_loopExpr->propagateTypes(ctx);    
    if (m_body) m_body->propagateTypes(ctx);
    
    m_labelTop->propagateTypes(ctx);
    m_labelContinue->propagateTypes(ctx);
    m_labelEnd->propagateTypes(ctx);
    m_loopGoto->propagateTypes(ctx);
    
    ctx->popParent();
}
    
void IrWhileStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "While(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;

    m_loopExpr->print(depth+1);
    if (m_body) m_body->print(depth+1);
}

bool IrWhileStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
        
    if (m_loopExpr->getType() != IrType::Boolean)
    {
        std::stringstream msg;
        msg << "while loop expression must be of type boolean.  Got: " << IrTypeToString(m_loopExpr->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }
        
    if (!m_loopExpr->analyze(ctx))
        valid = false;
    
    if (m_body) 
    {
        if (!m_body->analyze(ctx))
            valid = false;
    }
       
    m_labelTop->analyze(ctx);
    m_labelContinue->analyze(ctx);
    m_labelEnd->analyze(ctx);
    m_loopGoto->analyze(ctx);
       
    ctx->popParent();
    
    return valid;
}

bool IrWhileStatement::allocate(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
        
    if (m_body) 
    {
        if (!m_body->allocate(ctx))
            valid = false;
    }

    m_labelTop->allocate(ctx);
    m_labelContinue->allocate(ctx);
    m_labelEnd->allocate(ctx);
    m_loopExpr->allocate(ctx);
    m_loopGoto->allocate(ctx);
        
    ctx->popParent();
    
    return valid;
}

bool IrWhileStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
         
    m_labelTop->codegen(ctx);
    
    IrTacStmt label(IrOpcode::LABEL, getLineNumber());
    label.m_src0.build(m_labelTop.get());
    ctx->append(label);
     
    if (!m_loopExpr->codegen(ctx))
        valid = false;
    
    IrTacStmt tac(IrOpcode::IFZ, getLineNumber());
    tac.m_src0.build(m_loopExpr->getResult().get());
    tac.m_src1.build(m_labelEnd.get());
    ctx->append(tac);
      
    if (m_body) 
    {
        if (!m_body->codegen(ctx))
            valid = false;
    }
    m_labelContinue->codegen(ctx);
    label.m_opcode = IrOpcode::LABEL;
    label.m_src0.build(m_labelContinue.get());
    ctx->append(label);
    
    m_loopGoto->codegen(ctx);
    
    m_labelEnd->codegen(ctx);
    label.m_opcode = IrOpcode::LABEL;
    label.m_src0.build(m_labelEnd.get());
    ctx->append(label);
    
    ctx->popParent();
    
    return valid;
}

size_t IrWhileStatement::getAllocationSize() const
{
    size_t allocSize = 0;
    if (m_body != nullptr)
    {
        allocSize += m_body->getAllocationSize();
    }
    return allocSize;
}

void IrWhileStatement::setSymbolStartAddress(size_t addr)
{
    if (m_body != nullptr)
    {
        m_body->setSymbolStartAddress(addr);
    }
}
 
} // namespace Decaf
