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
#include "IrFieldDecl.h"
#include "IrMethodDecl.h"
#include "IrSymbolTable.h"

namespace Decaf
{

class IrClass : public IrBase
{
public:
    IrClass(int lineNumber, int columnNumber, const std::string& filename, IrIdentifierPtr ident) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_extends(nullptr),
        m_implements(),
        m_field_decl_list(),
        m_method_decl_list(),
        m_symbols(new IrSymbolTable())
    {
    }

    IrClass(int lineNumber, int columnNumber, const std::string& filename, IrIdentifierPtr ident, IrIdentifierPtr extends, std::vector<IrIdentifier*>* implements = nullptr) :
        IrBase(lineNumber, columnNumber, filename),
        m_identifier(ident),
        m_extends(extends),
        m_implements(),
        m_field_decl_list(),
        m_method_decl_list(),
        m_symbols(new IrSymbolTable())
    {
        if (implements)
        {
            for (auto it : *implements)
            {
                m_implements.push_back(IrIdentifierPtr(it));
            }
        }
    }
    
    virtual ~IrClass();
    
    virtual void propagateTypes(IrTraversalContext* ctx); 
    virtual void print(unsigned int depth); 
    virtual bool analyze(IrTraversalContext* ctx);
    virtual bool allocate(IrTraversalContext* ctx);       
    virtual bool codegen(IrTraversalContext* ctx);
    virtual const std::string& asString() const { return m_class; }
     
    void addFieldDecl(IrFieldDeclPtr field);
    void addFieldDecl(const std::vector<IrFieldDecl*>& fields);
    
    void addMethodDecl(IrMethodDeclPtr method);
    void addMethodDecl(const std::vector<IrMethodDecl*>& methods);
    
    IrSymbolTable* getSymbols()
    {
        return m_symbols.get();
    }
    
    IrIdentifierPtr getIdentifier() { return m_identifier; }
    
    size_t getNumMethods() const { return m_method_decl_list.size(); }
    IrMethodDeclPtr getMethod(size_t which) { return m_method_decl_list.at(which); }
    
protected:
    
    IrIdentifierPtr m_identifier;
    IrIdentifierPtr m_extends;
    std::vector<IrIdentifierPtr> m_implements;
    
    std::vector<IrFieldDeclPtr> m_field_decl_list;
    std::vector<IrMethodDeclPtr> m_method_decl_list;
    
    std::unique_ptr<IrSymbolTable> m_symbols;
    
    const std::string m_class = "class";
    
private:
    IrClass() = delete;
    IrClass(const IrClass& rhs) = delete;
};

} // namespace Decaf
