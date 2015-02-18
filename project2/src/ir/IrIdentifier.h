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
#include <string>
#include <vector>
#include "IrBase.h"

namespace Decaf
{

class IrIdentifier : public IrBase
{
public:
    IrIdentifier(int lineNumber, int columnNumber, const std::string& ident) :
        IrBase(lineNumber, columnNumber),
        m_identifier(ident),
        m_index(-1)
    {}

    IrIdentifier(int lineNumber, int columnNumber, const std::string& ident, int index) :
        IrBase(lineNumber, columnNumber),
        m_identifier(ident),
        m_index(index)
    {}
    
    virtual ~IrIdentifier()
    {}
    
    virtual void print() 
    {
        std::cout << "Identifier: " << m_identifier << " at " << getLineNumber() << ", " << getColumnNumber() << std::endl;
    }
    
    const std::string& getIdentifier() const { return m_identifier; }
    int getIndex() const { return m_index; }
    
protected:
    
    std::string m_identifier;
    int m_index;
    
private:
    IrIdentifier() = delete;
    IrIdentifier(const IrIdentifier& rhs) = delete;
};

} // namespace Decaf
