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
#include "IrIfStmt.h"
#include "IrExpression.h"
#include "IrBlock.h"
#include "IrBoolLiteral.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrIfStatement::~IrIfStatement()
{
    delete m_condition;
    delete m_trueBlock;
    delete m_falseBlock;
    
    delete m_labelFalse;
    delete m_labelEnd;
}
    
void IrIfStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushParent(this);

    m_condition->propagateTypes(ctx);
    if (m_trueBlock) m_trueBlock->propagateTypes(ctx);
    if (m_falseBlock) m_falseBlock->propagateTypes(ctx);
    
    ctx->popParent();    
}
    
void IrIfStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "If(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    m_condition->print(depth+1);
    if (m_trueBlock) m_trueBlock->print(depth+1);
    if (m_falseBlock) m_falseBlock->print(depth+1);
}
    
bool IrIfStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushParent(this);
    
    if (!m_condition->analyze(ctx))
        valid = false;
    
    if (m_trueBlock)
    {
        if (!m_trueBlock->analyze(ctx))
            valid = false;
    }
    if (m_falseBlock)
    {
        if (!m_falseBlock->analyze(ctx))
            valid = false;
    }
    
    // Condition express must be of type boolean.
    if (m_condition->getType() != IrType::Boolean)
    {
        std::stringstream msg;        
        msg <<  "if conditional expression must be of type boolean.  Got: " << IrTypeToString(m_condition->getType()) << ".";
        ctx->error(this, msg.str());
        valid = false;
    }
    
    ctx->popParent();
    
    return valid;
}

bool IrIfStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    // Template:
    // <evaluate condition>
    // jump<oper> <label_false>
    // <true_body>
    // jmp <label_end>
    // label_false:
    // <false_body>
    // label_end:
    
    ctx->pushParent(this);
    
    if (!m_condition->codegen(ctx))
        valid = false;
    
    delete m_labelFalse;
    m_labelFalse = IrIdentifier::CreateLabel();
    
    delete m_labelEnd;
    m_labelEnd = IrIdentifier::CreateLabel();
    
    IrTacStmt tac;
    tac.m_opcode = IrOpcode::IFZ;
    IrBooleanLiteral* literal = dynamic_cast<IrBooleanLiteral*>(m_condition);
    if (literal)
    {
        tac.m_arg0 = m_condition;
    }
    else
    {
        tac.m_arg0 = m_condition->getResultIdentifier();
    }
    tac.m_arg1 = m_labelFalse;
    tac.m_arg2 = nullptr;
    
    ctx->append(tac);
    
    if (m_trueBlock)
    {
        if (!m_trueBlock->codegen(ctx))
            valid = false;
        
        IrTacStmt jump;
        jump.m_opcode = IrOpcode::JUMP;
        jump.m_arg0 = m_labelEnd;
        
        ctx->append(jump);
    }
            
    IrTacStmt label;
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelFalse;
    ctx->append(label);
    
    if (m_falseBlock)
    {              
        if (!m_falseBlock->codegen(ctx))
            valid = false;
    }
    
    IrTacStmt elabel;
    elabel.m_opcode = IrOpcode::LABEL;
    elabel.m_arg0 = m_labelEnd;
    
    ctx->append(elabel);

    ctx->popParent();
    
    return valid;
}

size_t IrIfStatement::getAllocationSize() const
{
    size_t allocSize = 0;
    if (m_trueBlock != nullptr)
    {
        allocSize += m_trueBlock->getAllocationSize();
    }
    if (m_falseBlock != nullptr)
    {
        allocSize += m_falseBlock->getAllocationSize();
    }
    return allocSize;
}

void IrIfStatement::setSymbolStartAddress(size_t addr)
{
	if (m_trueBlock != nullptr)
    {
        m_trueBlock->setSymbolStartAddress(addr);
        addr += m_trueBlock->getAllocationSize();
    }
	if (m_falseBlock != nullptr)
    {
        m_falseBlock->setSymbolStartAddress(addr);
        //addr += m_falseBlock->getAllocationSize();
    }
}
 
} // namespace Decaf
