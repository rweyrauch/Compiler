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
#include "IrCommon.h"
#include "IrBase.h"
#include "IrLocation.h"
#include "IrIdentifier.h"

namespace Decaf
{

class IrFieldDecl : public IrBase
{
public:
    IrFieldDecl(int lineNumber, int columnNumber, const std::string& filename, IrLocation* location, IrType type) :
        IrBase(lineNumber, columnNumber, filename),
        m_location(location),
        m_type(type),
        m_typeIdent(nullptr)
    {
        m_location->setType(type);
    }
    IrFieldDecl(int lineNumber, int columnNumber, const std::string& filename, IrLocation* location, IrIdentifier* type) :
        IrBase(lineNumber, columnNumber, filename),
        m_location(location),
        m_type(IrType::Class),
        m_typeIdent(type)
    {
        m_location->setType(m_type);
        m_location->setTypeIdent(m_typeIdent);
    }
    
    virtual ~IrFieldDecl()
    {}
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth);
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
   
    IrLocation* getLocation() const { return m_location.get(); }
    const std::string& getName() const { return m_location->getIdentifier()->getIdentifier(); }
    IrType getType() const { return m_type; }
    std::shared_ptr<IrIdentifier> getTypeIdentifier() const { return m_typeIdent; }
    
protected:
    
    std::shared_ptr<IrLocation> m_location;
    IrType m_type; 
    std::shared_ptr<IrIdentifier> m_typeIdent;
    
private:
    IrFieldDecl() = delete;
    IrFieldDecl(const IrFieldDecl& rhs) = delete;
};

} // namespace Decaf
