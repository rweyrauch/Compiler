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
#include "IrBase.h"
#include "IrIdentifier.h"
#include "IrMethodDecl.h"

namespace Decaf
{

class IrInterface : public IrBase
{
public:
    IrInterface(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* ident) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_method_decl_list()
    {
    }
    
    virtual ~IrInterface();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth); 
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx) { return true; }   
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_interface; }
    
    void addMethodDecl(IrMethodDecl* method);
    void addMethodDecl(const std::vector<IrMethodDecl*>& methods);
    
protected:
    
    std::shared_ptr<IrIdentifier> m_identifier;
    
    std::vector<std::shared_ptr<IrMethodDecl>> m_method_decl_list;
    const std::string m_interface = "interface";
    
private:
    IrInterface() = delete;
    IrInterface(const IrInterface& rhs) = delete;
};

} // namespace Decaf
