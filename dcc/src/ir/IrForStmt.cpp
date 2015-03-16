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
    m_loopAuto(nullptr),
    m_initialValue(initialExpr),
    m_terminatingValue(endExpr),
    m_body(block),
    m_symbols(nullptr)
{
	m_loopAuto = new IrLocation(lineNumber, columnNumber, filename, loopVar, IrType::Integer);
	
	m_initLoopAuto = new IrAssignExpression(lineNumber, columnNumber, filename, m_loopAuto, IrAssignmentOperator::Assign, m_initialValue);
	m_labelTop = IrIdentifier::CreateLabel();
	m_labelEnd = IrIdentifier::CreateLabel();
	
	m_terminatingExpr = new IrBooleanExpression(lineNumber, columnNumber, filename, m_loopAuto, IrBooleanOperator::LessEqual, m_terminatingValue);
	
	m_loopIncrement = new IrIntegerLiteral(lineNumber, columnNumber, filename, "1");
	m_incrementLoop = new IrAssignExpression(lineNumber, columnNumber, filename, m_loopAuto, IrAssignmentOperator::IncrementAssign, m_loopIncrement);

	m_loopGoto = new IrGotoStatement(lineNumber, columnNumber, filename, m_labelTop);
	
    m_symbols = new IrSymbolTable();
    m_symbols->addVariable(m_loopAuto);
}
    
IrForStatement::~IrForStatement()
{
    delete m_loopAuto;
    delete m_initialValue;
    delete m_terminatingValue;
    delete m_body;
    delete m_symbols;
}
    
void IrForStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopAuto->propagateTypes(ctx);    
    m_initialValue->propagateTypes(ctx);
    m_terminatingValue->propagateTypes(ctx);
    
    if (m_body) m_body->propagateTypes(ctx);
    
    ctx->popParent();
    ctx->popSymbols();
}
    
void IrForStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "For(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    m_loopAuto->print(depth+1);
    m_initialValue->print(depth+1);
    m_terminatingValue->print(depth+1);
    
    if (m_body) m_body->print(depth+1);
}

bool IrForStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    if (!m_loopAuto->analyze(ctx))
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

    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

bool IrForStatement::codegen(IrTraversalContext* ctx)
{
    bool valid = true;
    
    // int loop_var = <initial value>
    // LABEL_CONT:
    // if (loop_var < term_var)
    // {
    //     <body>
    //     loop_var += 1;
    //     goto LABEL_CONT;
    // }
    
    ctx->pushSymbols(m_symbols);
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
    m_incrementLoop->codegen(ctx);
    m_loopGoto->codegen(ctx);
    
    m_labelEnd->codegen(ctx);
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelEnd;
    ctx->append(label);
     
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

} // namespace Decaf
