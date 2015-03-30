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

class IrAddress : public IrBase
{
public:
    IrAddress(std::shared_ptr<IrIdentifier> identifier) :
        IrBase(0, 0, __FILE__),
        m_identifier(identifier),
        m_offset(nullptr)
    {}
        
    IrAddress(std::shared_ptr<IrIdentifier> identifier, std::shared_ptr<IrIdentifier> offset) :
        IrBase(0, 0, __FILE__),
        m_identifier(identifier),
        m_offset(offset)
    {}

    virtual void print(unsigned int depth) {} 
    virtual bool codegen(IrTraversalContext* ctx)
    {
        bool valid = true;
        if (m_identifier) 
            if (!m_identifier->codegen(ctx)) valid = false;
        if (m_offset)
            if (!m_offset->codegen(ctx)) valid = false;
            
        return valid;
    }
    
    std::shared_ptr<IrIdentifier> getIdentifier() const { return m_identifier; }
    std::shared_ptr<IrIdentifier> getOffset() const { return m_offset; }
    
protected:
    std::shared_ptr<IrIdentifier> m_identifier;
    std::shared_ptr<IrIdentifier> m_offset;
};


class IrExpression : public IrBase
{
public:
    IrExpression(int lineNumber, int columnNumber, const std::string& filename, IrType type) :
        IrBase(lineNumber, columnNumber, filename),
        m_type(type),
        m_isArray(false),
        m_result(nullptr)
    {}
    
    virtual ~IrExpression()
    {}
    
    virtual void print(unsigned int depth) = 0;
    
    void setType(IrType type, bool isArray = false) { m_type = type; m_isArray = isArray; }
    IrType getType() const { return m_type; }
    bool isArray() const { return m_isArray; }
    
    std::shared_ptr<IrAddress> getResult() const { return m_result; }
    
protected:
    
    IrType m_type;
    bool m_isArray;
    
    std::shared_ptr<IrAddress> m_result;
    
private:
    IrExpression() = delete;
    IrExpression(const IrExpression& rhs) = delete;
};

} // namespace Decaf

