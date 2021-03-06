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
#pragma once
#include <vector>
#include <memory>
#include "IrCommon.h"
#include "IrBase.h"
#include "IrBlock.h"
#include "IrIdentifier.h"
#include "IrSymbolTable.h"

namespace Decaf
{
class IrVariableDecl;
class IrIntegerLiteral;

class IrMethodDecl : public IrBase
{
public:
    IrMethodDecl(int lineNumber, int columnNumber, const std::string& filename, IrIdentifierPtr ident, IrType returnType) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_returnType(returnType),
        m_argument_list(),
        m_block(nullptr),
        m_symbols(new IrSymbolTable()),
        m_stackSize(0)
    {
    }
    
    virtual ~IrMethodDecl();
     
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);       
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const;
     
    void addArgument(IrVariableDeclPtr arg)
    {
        m_argument_list.push_back(arg);
        m_symbols->addVariable(arg.get());
    }
    void addBlock(IrBlockPtr block)
    {
        m_block = block;
    }
    
    const std::string& getName() const { return m_identifier->getIdentifier(); }
    size_t getNumArguments() const { return m_argument_list.size(); }
    IrVariableDeclPtr getArgument(size_t which) { return m_argument_list.at(which); }
    IrType getReturnType() const { return m_returnType; }
    
protected:
    
    IrIdentifierPtr m_identifier;
    IrType m_returnType;
    std::vector<IrVariableDeclPtr> m_argument_list;
    IrBlockPtr m_block;
    std::unique_ptr<IrSymbolTable> m_symbols;
    size_t m_stackSize;
 
private:
    IrMethodDecl() = delete;
    IrMethodDecl(const IrMethodDecl& rhs) = delete;
};

} // namespace Decaf
