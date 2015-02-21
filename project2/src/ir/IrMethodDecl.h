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
#include "IrCommon.h"
#include "IrBase.h"
#include "IrIdentifier.h"

namespace Decaf
{
class IrVariableDecl;
class IrBlock;

class IrMethodDecl : public IrBase
{
public:
    IrMethodDecl(int lineNumber, int columnNumber, IrIdentifier* ident, IrType returnType) :
        IrBase(lineNumber, columnNumber),
        m_identifier(ident),
        m_returnType(returnType),
        m_argument_list(),
        m_block(nullptr)
    {}
    
    virtual ~IrMethodDecl()
    {}
    
    virtual void print(unsigned int depth);
     
    void addArgument(IrVariableDecl* arg)
    {
        m_argument_list.push_back(arg);
    }
    void addBlock(IrBlock* block)
    {
        m_block = block;
    }
    
    const std::string& getName() const { return m_identifier->getIdentifier(); }
    size_t getNumArguments() const { return m_argument_list.size(); }
    IrType getReturnType() const { return m_returnType; }
    
protected:
    
    IrIdentifier* m_identifier;
    IrType m_returnType;
    std::vector<IrVariableDecl*> m_argument_list;
    IrBlock* m_block;
    
private:
    IrMethodDecl() = delete;
    IrMethodDecl(const IrMethodDecl& rhs) = delete;
};

} // namespace Decaf
