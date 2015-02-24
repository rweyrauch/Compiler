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
#include <list>
#include <vector>
#include "IrCommon.h"
#include "IrBase.h"

namespace Decaf
{
class IrSymbolTable;
class IrLocation;
class IrMethodCall;

class IrTraversalContext
{
public:
    IrTraversalContext() {}
    ~IrTraversalContext() {}
    
    void pushSymbols(IrSymbolTable* symbols) { m_symbols.push_front(symbols); }
    void popSymbols() { m_symbols.pop_front(); }
    
    void pushParent(IrBase* parent) { m_parents.push_back(parent); }
    void popParent() { m_parents.pop_back(); }
    
    bool lookup(IrLocation* variable, SVariableSymbol& symbol) const;
    bool lookup(IrMethodCall* method, SMethodSymbol& symbol) const;
    
protected:
    
    std::list<IrSymbolTable*> m_symbols;
    std::vector<IrBase*> m_parents;
};

} // namespace Decaf
