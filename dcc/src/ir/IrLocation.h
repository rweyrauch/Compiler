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
#include "IrExpression.h"

namespace Decaf
{
class IrIdentifier;

class IrLocation : public IrExpression
{
public:
    
    IrLocation(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* ident, IrType type) :
        IrExpression(lineNumber, columnNumber, filename, type),
        m_identifier(ident),
        m_index(nullptr),
        m_asDeclaration(false)
    {}

    IrLocation(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* ident, IrType type, IrExpression* index) :
        IrExpression(lineNumber, columnNumber, filename, type),
        m_identifier(ident),
        m_index(index),
        m_asDeclaration(false)
    {}
    
    virtual ~IrLocation()
    {}
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
    
    void setAsDeclaration() { m_asDeclaration = true; }
    bool usedAsDeclaration() const { return m_asDeclaration; }
    
    IrIdentifier* getIdentifier() const { return m_identifier; }
    IrExpression* getIndex() const { return m_index; }
    
protected:
    
    IrIdentifier* m_identifier;
    IrExpression* m_index;
    bool m_asDeclaration;
    
private:
    IrLocation() = delete;
    IrLocation(const IrLocation& rhs) = delete;
};

} // namespace Decaf

