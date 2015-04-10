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
#include <iostream>
#include <sstream>
#include "IrCommon.h"
#include "IrLiteral.h"

namespace Decaf
{
 
class IrDoubleLiteral : public IrLiteral
{
public:
    IrDoubleLiteral(int lineNumber, int columnNumber, const std::string& filename, const std::string& value) :
        IrLiteral(lineNumber, columnNumber, filename, IrType::Double, value)
    {
        std::stringstream conv(value);
        conv >> m_value;
    }
    
    virtual ~IrDoubleLiteral()
    {}
    
    virtual void print(unsigned int depth) 
    {
        IRPRINT_INDENT(depth);
        std::cout << "Double(" << getLineNumber() << "," << getColumnNumber() << ") = " << getValue() << std::endl;
    }
    
    double getValue() const { return m_value; }
    void setValue(double value) { m_value = value; }

protected:
    
    double m_value;
    
private:
    IrDoubleLiteral() = delete;
    IrDoubleLiteral(const IrDoubleLiteral& rhs) = delete;
};

}