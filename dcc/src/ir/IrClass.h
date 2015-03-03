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
#include "IrBase.h"
#include "IrIdentifier.h"
#include "IrFieldDecl.h"
#include "IrMethodDecl.h"
#include "IrSymbolTable.h"

namespace Decaf
{

class IrClass : public IrBase
{
public:
    IrClass(int lineNumber, int columnNumber, const std::string& filename, IrIdentifier* ident) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_field_decl_list(),
        m_method_decl_list(),
        m_symbols(nullptr)
    {
        m_symbols = new IrSymbolTable();
    }
    
    virtual ~IrClass();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth); 
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool codegen(IrTraversalContext* ctx);
     
    void addFieldDecl(IrFieldDecl* field);
    void addFieldDecl(const std::vector<IrFieldDecl*>& fields);
    
    void addMethodDecl(IrMethodDecl* method);
    void addMethodDecl(const std::vector<IrMethodDecl*>& methods);
    
    IrSymbolTable* getSymbols()
    {
        return m_symbols;
    }
    
protected:
    
    IrIdentifier* m_identifier;
    
    std::vector<IrFieldDecl*> m_field_decl_list;
    std::vector<IrMethodDecl*> m_method_decl_list;
    
    IrSymbolTable* m_symbols;
    
private:
    IrClass() = delete;
    IrClass(const IrClass& rhs) = delete;
};

} // namespace Decaf