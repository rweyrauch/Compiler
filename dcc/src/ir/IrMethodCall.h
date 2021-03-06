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
#include <string>
#include <vector>
#include "IrExpression.h"

namespace Decaf
{
class IrIdentifier;
class IrStringLiteral;

class IrMethodCall : public IrExpression
{
public:
    IrMethodCall(int lineNumber, int columnNumber, const std::string& filename, IrIdentifierPtr ident, IrType type) :
        IrExpression(lineNumber, columnNumber, filename, type),
        m_identifier(ident),
        m_externFuncName(nullptr),
        m_externalFunction(false),
        m_arguments()
    {}

    IrMethodCall(int lineNumber, int columnNumber, const std::string& filename, IrStringLiteralPtr name, IrType type);
    
    virtual ~IrMethodCall();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const;
    
    void addArgument(IrExpressionPtr arg)
    {
        m_arguments.push_back(arg);
    }
    
    bool isExternal() const { return m_externalFunction; }
    const IrIdentifierPtr getIdentifier() const { return m_identifier; }
    size_t getNumArguments() const { return m_arguments.size(); }
    IrType getArgumentType(size_t which) const { return m_arguments.at(which)->getType(); }
    
protected:    
    
    IrIdentifierPtr m_identifier;
    IrStringLiteralPtr m_externFuncName;
    bool m_externalFunction;
    std::vector<IrExpressionPtr> m_arguments;
    
private:
    IrMethodCall() = delete;
    IrMethodCall(const IrMethodCall& rhs) = delete;
};

} // namespace Decaf
