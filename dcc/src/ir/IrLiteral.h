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
 
class IrLiteral : public IrExpression
{
public:
    IrLiteral(int lineNumber, int columnNumber, const std::string& filename, IrType type, const std::string& value) :
        IrExpression(lineNumber, columnNumber, filename, type),
        m_valueAsString(value)
    {}
    
    virtual ~IrLiteral()
    {}
    
    virtual void print(unsigned int depth) = 0;
    virtual const std::string& asString() const { return getValueAsString(); };
    
    const std::string& getValueAsString() const { return m_valueAsString; }
    
protected:
    
    std::string m_valueAsString;
    
private:
    IrLiteral() = delete;
    IrLiteral(const IrLiteral& rhs) = delete;
};

}
