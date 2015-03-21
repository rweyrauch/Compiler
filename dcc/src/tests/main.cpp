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
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <popt.h>
#include <string>
#include <stdexcept>
#include <cstdio>

#include "SymbolTableTests.h"

static int g_listTests = 0;

#ifndef _MSC_VER
static poptOption optionsTable[] =
{
    { "list", 'l', POPT_ARG_NONE, &g_listTests, 0, "List available tests.", NULL },
    POPT_AUTOALIAS
    POPT_AUTOHELP
    POPT_TABLEEND
};
#endif

int main(int argc, const char** argv)
{
    // parse command lines args
    poptContext context = poptGetContext("TestDccIR", argc, argv, optionsTable, 0);
    poptGetNextOpt(context);

    CppUnit::TextTestRunner runner;

    std::vector<CppUnit::Test*> unitTests;
    
    unitTests.push_back(SymbolTableTestCase::suite());
    
    if (g_listTests)
    {
        std::vector<CppUnit::Test*>::iterator it = unitTests.begin();
        std::cout << "Available Test Suites:" << std::endl;
        while (it != unitTests.end())
        {
            std::cout << "    " << (*it)->getName() << std::endl;
            ++it;
        }
        std::cout << std::endl;
    }
    else
    {
        std::vector<CppUnit::Test*>::iterator it = unitTests.begin();
        while (it != unitTests.end())
        {
            runner.addTest(*it);
            ++it;
        }
        if (poptPeekArg(context))
        {
            std::cout << "Running single test: " << poptPeekArg(context) << std::endl;
            runner.run(poptGetArg(context));
        }
        else
        {
            std::cout << "Running all tests..." << std::endl;
            runner.run();
        }
    }

    poptFreeContext(context);

    bool success = runner.result().wasSuccessful();
    return success ? 0 : 1;
}
