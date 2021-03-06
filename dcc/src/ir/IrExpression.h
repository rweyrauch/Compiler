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
#include "IrIdentifier.h"

namespace Decaf
{

class IrExpression : public IrBase
{
public:
    IrExpression(int lineNumber, int columnNumber, const std::string& filename, IrType type) :
        IrBase(lineNumber, columnNumber, filename),
        m_type(type),
        m_isArray(false),
        m_asWrite(false),
        m_result(nullptr)
    {}
    
    virtual ~IrExpression()
    {}
    
    virtual void print(unsigned int depth) = 0;
    
    void setType(IrType type, bool isArray = false) { m_type = type; m_isArray = isArray; }
    IrType getType() const { return m_type; }
    bool isArray() const { return m_isArray; }
    
    void setAsWrite(bool write ) { m_asWrite = write; }
    bool usedAsWrite() const { return m_asWrite; }
    
    IrIdentifierPtr getResult() const { return m_result; }
    
protected:
    
    IrType m_type;
    bool m_isArray;
    bool m_asWrite;
    
    IrIdentifierPtr m_result;
    
private:
    IrExpression() = delete;
    IrExpression(const IrExpression& rhs) = delete;
};

} // namespace Decaf

