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
#include "IrCommon.h"
#include "IrBase.h"

namespace Decaf
{
class IrTraversalContext;

class IrIdentifier : public IrBase
{
public:
       
    static IrIdentifierPtr CreateTemporary();    
    static IrIdentifierPtr CreateLabel();
    
    IrIdentifier(int lineNumber, int columnNumber, const std::string& filename, const std::string& ident, bool isLabel = false) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_addr(0),
        m_isLabel(isLabel),
        m_isGlobal(false),
        m_type(IrType::Unknown)
    {}
    
    virtual ~IrIdentifier()
    {}
    
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);   
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return getIdentifier(); };
    
    const std::string& getIdentifier() const { return m_identifier; }
    ptrdiff_t getAddress() const { return m_addr; }
    bool isLabel() const { return m_isLabel; }
    bool isGlobal() const { return m_isGlobal; }
    IrType getType() const { return m_type; }
    
protected:
    
    std::string m_identifier;
    ptrdiff_t m_addr;
    bool m_isLabel;
    bool m_isGlobal;
    IrType m_type;
    
    static int s_tempCounter;
    
private:
    IrIdentifier() = delete;
    IrIdentifier(const IrIdentifier& rhs) = delete;
};

} // namespace Decaf
