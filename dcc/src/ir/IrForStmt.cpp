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
#include "IrIntLiteral.h"
#include "IrBlock.h"
#include "IrTravCtx.h"

namespace Decaf
{

IrForStatement::IrForStatement(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* loopVar, IrExpression* initialExpr, IrExpression* endExpr, IrBlock* block) :
    IrStatement(lineNumber, columnNumber, filename),
    m_loopAuto(nullptr),
    m_loopVarExpr(nullptr),
    m_initialValue(initialExpr),
    m_terminatingValue(endExpr),
    m_incrementExpr(nullptr),
    m_terminatingExpr(nullptr),
    m_body(block),
    m_symbols(nullptr),
    m_labelCont(nullptr),
    m_labelEnd(nullptr)
{
    m_loopAuto = new IrLocation(loopVar->getLineNumber(), loopVar->getColumnNumber(), filename, loopVar, IrType::Integer);
    m_incrementExpr = new IrIntegerLiteral(loopVar->getLineNumber(), loopVar->getColumnNumber(), filename, "1");
    m_loopVarExpr = new IrBinaryExpression(loopVar->getLineNumber(), loopVar->getColumnNumber(), filename, m_loopAuto->getType(), m_loopAuto, IrBinaryOperator::Add, m_incrementExpr);
    
    delete m_labelCont;
    m_labelCont = IrIdentifier::CreateLabel();
    
    delete m_labelEnd;
    m_labelEnd = IrIdentifier::CreateLabel();
    
    delete m_terminatingExpr;
    m_terminatingExpr = new IrBooleanExpression(0, 0, __FILE__, m_loopVarExpr, IrBooleanOperator::GreaterEqual, m_terminatingValue);
    
    m_symbols = new IrSymbolTable();
    m_symbols->addVariable(m_loopAuto);
}
    
IrForStatement::~IrForStatement()
{
    delete m_loopAuto;
    delete m_initialValue;
    delete m_terminatingValue;
    delete m_incrementExpr;
    delete m_terminatingExpr;
    delete m_body;
    delete m_symbols;
    
    delete m_labelCont;
    delete m_labelEnd;    
}
    
void IrForStatement::propagateTypes(IrTraversalContext* ctx)
{
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopAuto->propagateTypes(ctx);
    m_loopVarExpr->propagateTypes(ctx);    
    m_initialValue->propagateTypes(ctx);
    m_terminatingValue->propagateTypes(ctx);
    m_incrementExpr->propagateTypes(ctx);
    
    if (m_body) m_body->propagateTypes(ctx);
    
    ctx->popParent();
    ctx->popSymbols();
}
    
void IrForStatement::print(unsigned int depth) 
{
    IRPRINT_INDENT(depth);
    std::cout << "For(" << getLineNumber() << "," << getColumnNumber() << ")" << std::endl;
    
    m_loopAuto->print(depth+1);
    m_loopVarExpr->print(depth+1);
    m_initialValue->print(depth+1);
    m_terminatingValue->print(depth+1);
    m_incrementExpr->print(depth+1);
    
    if (m_body) m_body->print(depth+1);
}

bool IrForStatement::analyze(IrTraversalContext* ctx)
{
    bool valid = true;
    
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    if (!m_loopAuto->analyze(ctx))
        valid = false;
    if (!m_loopVarExpr->analyze(ctx))
        valid = false;    
    if (!m_initialValue->analyze(ctx))
        valid = false;
    if (!m_terminatingValue->analyze(ctx))
        valid = false;
    if (!m_incrementExpr->analyze(ctx))
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
    // int term_var = <terminating value>
    // LABEL_CONT:
    // if (loop_var < term_var)
    // {
    //     <body>
    //     loop_var += 1;
    //     goto LABEL_CONT;
    // }
    
    // Template:
    // init_var = <evaluate initial value>
    // loop_var = <evaluate terminating value>
    // label_cont:
    // temp = <evaluate termination expression>
    // ifz temp <label_end>
    // <body>
    // incr loop_var
    // jmp <label_cont>
    // label_end:
    
    ctx->pushSymbols(m_symbols);
    ctx->pushParent(this);
    
    m_loopAuto->codegen(ctx);
    m_initialValue->codegen(ctx);
        
    IrTacStmt label;  
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelCont;
    ctx->append(label);
    
    m_terminatingValue->codegen(ctx);
    
    m_terminatingExpr->codegen(ctx);
    
    IrTacStmt termLoop;
    termLoop.m_opcode = IrOpcode::IFZ;
    termLoop.m_arg0 = m_terminatingExpr;
    termLoop.m_arg1 = m_labelEnd;
    
    ctx->append(termLoop);
    
    if (m_body) m_body->codegen(ctx);
    
    m_incrementExpr->codegen(ctx);

    IrTacStmt jump;
    jump.m_opcode = IrOpcode::JUMP;
    jump.m_arg0 = m_labelCont;
    
    ctx->append(jump);
    
    label.m_opcode = IrOpcode::LABEL;
    label.m_arg0 = m_labelEnd;
    ctx->append(label);
    
    ctx->popParent();
    ctx->popSymbols();
    
    return valid;
}

} // namespace Decaf
