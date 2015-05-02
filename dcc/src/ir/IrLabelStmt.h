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
#include <memory>
#include "IrStatement.h"

namespace Decaf
{
class IrIdentifier;

class IrLabelStatement : public IrStatement
{
public:
    IrLabelStatement(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* label) :
        IrStatement(lineNumber, columnNumber, filename),
        m_label(std::shared_ptr<IrIdentifier>(label))
    {}
    
    virtual ~IrLabelStatement()
    {}
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx) { return true; }
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const;
    
protected:    
    std::shared_ptr<IrIdentifier> m_label;
    
private:
    IrLabelStatement() = delete;
    IrLabelStatement(const IrLabelStatement& rhs) = delete;
};

} // namespace Decaf
